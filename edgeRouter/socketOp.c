#include "threads.h"

// Function prototypes
unsigned short int checksum (unsigned short int *addr, int len);
unsigned short int icmp6_checksum (struct ip6_hdr, struct icmp6_hdr, unsigned char *, int);
void send_pingReply(sockData_t *reply);

void* thread_socketOp(void* nullIp)
{
   
   static sockData_t icmpReply;   // For now at least

   while(1)
   {
      memset(&icmpReply, 0, sizeof(sockData_t));
      sem_wait(&sockSem);
      // Semaphore is released. There is data available in queue 
      // Take mutex and acces data
      pthread_mutex_lock(&sockQMutex);

      /************************************/
      /**Process Socket Data queue **/
      /************************************/
      
      delete(SOCK_DATA_QUEUE, (void *)&icmpReply);
      pthread_mutex_unlock(&sockQMutex);
      
      send_pingReply(&icmpReply);
   }
}

void send_pingReply(sockData_t *reply)
{
  int i, status, datalen, frame_length, sd, bytes;
  char *interface, *target, *src_ip, *dst_ip;
  struct ip6_hdr iphdr;
  struct icmp6_hdr icmphdr;
  unsigned char *data, *src_mac, *ether_frame;
  unsigned char dst_mac[] = {0X43, 0xF2, 0XA3, 0xA1, 0XEB, 0x88};
  struct addrinfo hints, *res;
  struct sockaddr_in6 *ipv6;
  struct sockaddr_ll device;
  struct ifreq ifr;
  void *tmp;

// Allocate memory for various arrays.

  tmp = (unsigned char *) malloc (6 * sizeof (unsigned char));
  if (tmp != NULL) {
    src_mac = tmp;
  } else {
    fprintf (stderr, "ERROR: Cannot allocate memory for array 'src_mac'.\n");
    exit (EXIT_FAILURE);
  }
  memset (src_mac, 0, 6 * sizeof (unsigned char));

  // Maximum ICMP payload size = 65535 - IPv6 header (40 bytes) - ICMP header (8 bytes)
  tmp = (unsigned char *) malloc ((IP_MAXPACKET - IP6_HDRLEN - ICMP_HDRLEN) * sizeof (unsigned char));
  if (tmp != NULL) {
    data = tmp;
  } else {
    fprintf (stderr, "ERROR: Cannot allocate memory for array 'data'.\n");
    exit (EXIT_FAILURE);
  }
  memset (data, 0, (IP_MAXPACKET - IP6_HDRLEN - ICMP_HDRLEN) * sizeof (unsigned char));

  tmp = (unsigned char *) malloc (IP_MAXPACKET * sizeof (unsigned char));
  if (tmp != NULL) {
    ether_frame = tmp;
  } else {
    fprintf (stderr, "ERROR: Cannot allocate memory for array 'ether_frame'.\n");
    exit (EXIT_FAILURE);
  }
  memset (ether_frame, 0, IP_MAXPACKET * sizeof (unsigned char));

  tmp = (char *) malloc (40 * sizeof (char));
  if (tmp != NULL) {
    interface = tmp;
  } else {
    fprintf (stderr, "ERROR: Cannot allocate memory for array 'interface'.\n");
    exit (EXIT_FAILURE);
  }
  memset (interface, 0, 40 * sizeof (char));

  tmp = (char *) malloc (40 * sizeof (char));
  if (tmp != NULL) {
    target = tmp;
  } else {
    fprintf (stderr, "ERROR: Cannot allocate memory for array 'target'.\n");
    exit (EXIT_FAILURE);
  }
  memset (target, 0, 40 * sizeof (char));

  tmp = (char *) malloc (40 * sizeof (char));
  if (tmp != NULL) {
    src_ip = tmp;
  } else {
    fprintf (stderr, "ERROR: Cannot allocate memory for array 'src_ip'.\n");
    exit (EXIT_FAILURE);
  }
  memset (src_ip, 0, 40 * sizeof (char));

  tmp = (char *) malloc (40 * sizeof (char));
  if (tmp != NULL) {
    dst_ip = tmp;
  } else {
    fprintf (stderr, "ERROR: Cannot allocate memory for array 'dst_ip'.\n");
    exit (EXIT_FAILURE);
  }
  memset (dst_ip, 0, 40 * sizeof (char));

// Interface to send packet through.
  strcpy (interface, "wlan0");

// Submit request for a socket descriptor to lookup interface.
  if ((sd = socket (AF_INET6, SOCK_RAW, IPPROTO_RAW)) < 0) {
    perror ("socket() failed to get socket descriptor for using ioctl() ");
    exit (EXIT_FAILURE);
  }

// Use ioctl() to lookup interface and get MAC address.
  memset (&ifr, 0, sizeof (ifr));
  snprintf (ifr.ifr_name, sizeof (ifr.ifr_name), "%s", interface);
  if (ioctl (sd, SIOCGIFHWADDR, &ifr) < 0) {
    perror ("ioctl() failed to get source MAC address ");
    return;
  }
  close (sd);

// Copy source MAC address
  memcpy (src_mac, ifr.ifr_hwaddr.sa_data, 6);

// Report source MAC address to stdout.
  printf ("MAC address for interface %s is ", interface);
  for (i=0; i<5; i++) {
    printf ("%02x:", src_mac[i]);
  }
  printf ("%02x\n", src_mac[5]);

// Resolve interface index.
  if ((device.sll_ifindex = if_nametoindex (interface)) == 0) {
    perror ("if_nametoindex() failed to obtain interface index ");
    exit (EXIT_FAILURE);
  }
  printf ("Index for interface %s is %i\n", interface, device.sll_ifindex);

// Source IPv6 address: you need to fill this out
  strcpy (src_ip, SRC_IP_ADDR);

// Destination URL or IPv6 address
  strcpy (target, DEST_IP_ADDR);

// Fill out hints for getaddrinfo().
  memset (&hints, 0, sizeof (hints));
  hints.ai_family = AF_INET6;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = hints.ai_flags | AI_CANONNAME;

// Resolve target using getaddrinfo().
  if ((status = getaddrinfo (target, NULL, &hints, &res)) != 0) {
    fprintf (stderr, "getaddrinfo() failed %s\n", gai_strerror (status));
    exit (EXIT_FAILURE);
  }
  ipv6 = (struct sockaddr_in6 *) res->ai_addr;
  tmp = &(ipv6->sin6_addr);
  inet_ntop (AF_INET6, tmp, dst_ip, 40);
  freeaddrinfo (res);

// Fill out sockaddr_ll
  device.sll_family = AF_PACKET;
  device.sll_protocol = htons (ETH_P_IP);
  memcpy (device.sll_addr, dst_mac, 6);
  device.sll_halen = htons (6);

// ICMP data
  datalen = 4;
  data[0] = 'T';
  data[1] = 'e';
  data[2] = 's';
  data[3] = 't';

// IPv6 header

// IPv6 version (4 bits), Traffic class (8 bits), Flow label (20 bits)
  iphdr.ip6_flow = htonl ((6 << 28) | (0 << 20) | 0);

// Payload length (16 bits): ICMP header + ICMP data
  iphdr.ip6_plen = htons (ICMP_HDRLEN + datalen);

// Next header (8 bits): 58 for ICMP
  iphdr.ip6_nxt = IPPROTO_ICMPV6;

// Hop limit (8 bits): default to maximum value
  iphdr.ip6_hops = 255;

// Source IPv6 address (128 bits)
  inet_pton (AF_INET6, src_ip, &iphdr.ip6_src);

// Destination IPv6 address (128 bits)
  inet_pton (AF_INET6, dst_ip, &iphdr.ip6_dst);

// ICMP header

// Message Type (8 bits): echo request
  icmphdr.icmp6_type = ICMP6_ECHO_REQUEST;

// Message Code (8 bits): echo request
  icmphdr.icmp6_code = 0;

// Identifier (16 bits): usually pid of sending process - pick a number
  icmphdr.icmp6_id = htons (1000);

// Sequence Number (16 bits): starts at 0
  icmphdr.icmp6_seq = htons (0);

// ICMP header checksum (16 bits): set to 0 when calculating checksum
  icmphdr.icmp6_cksum = 0;
  icmphdr.icmp6_cksum = icmp6_checksum (iphdr, icmphdr, data, datalen);

// Fill out ethernet frame header.

// Ethernet frame length = ethernet data (IP header + ICMP header + ICMP data)
  frame_length = IP6_HDRLEN + ICMP_HDRLEN + datalen;

// IPv6 header
  memcpy (ether_frame, &iphdr, IP6_HDRLEN);

// ICMP header
  memcpy (ether_frame + IP6_HDRLEN, &icmphdr, ICMP_HDRLEN);

// ICMP data
  memcpy (ether_frame + IP6_HDRLEN + ICMP_HDRLEN, data, datalen);

  // Open raw socket descriptor.
  /* 
     if ((sd = socket (PF_PACKET, SOCK_DGRAM, htons (ETH_P_ALL))) < 0) {
     perror ("socket() failed ");
     exit (EXIT_FAILURE);
     }
  */
  sd = socketCommFd;

// Send ethernet frame to socket.
  if ((bytes = sendto (sd, ether_frame, frame_length, 0, (struct sockaddr *) &device, sizeof (device))) <= 0) {
    perror ("sendto() failed");
    exit (EXIT_FAILURE);
  }

// Free allocated memory.
  free (src_mac);
  free (data);
  free (ether_frame);
  free (interface);
  free (target);
  free (src_ip);
  free (dst_ip);

  return;
}

// Checksum function
unsigned short int
checksum (unsigned short int *addr, int len)
{
  int nleft = len;
  int sum = 0;
  unsigned short int *w = addr;
  unsigned short int answer = 0;

  while (nleft > 1) {
    sum += *w++;
    nleft -= sizeof (unsigned short int);
  }

  if (nleft == 1) {
    *(unsigned char *) (&answer) = *(unsigned char *) w;
    sum += answer;
  }

  sum = (sum >> 16) + (sum & 0xFFFF);
  sum += (sum >> 16);
  answer = ~sum;
  return (answer);
}

// Build IPv6 ICMP pseudo-header and call checksum function (Section 8.1 of RFC 2460).
unsigned short int
icmp6_checksum (struct ip6_hdr iphdr, struct icmp6_hdr icmp6hdr, unsigned char *payload, int payloadlen)
{
  char buf[IP_MAXPACKET];
  char *ptr;
  int chksumlen = 0;
  int i;

  ptr = &buf[0];  // ptr points to beginning of buffer buf

  // Copy source IP address into buf (128 bits)
  memcpy (ptr, &iphdr.ip6_src.s6_addr, sizeof (iphdr.ip6_src.s6_addr));
  ptr += sizeof (iphdr.ip6_src);
  chksumlen += sizeof (iphdr.ip6_src);

  // Copy destination IP address into buf (128 bits)
  memcpy (ptr, &iphdr.ip6_dst.s6_addr, sizeof (iphdr.ip6_dst.s6_addr));
  ptr += sizeof (iphdr.ip6_dst.s6_addr);
  chksumlen += sizeof (iphdr.ip6_dst.s6_addr);

  // Copy Upper Layer Packet length into buf (32 bits).
  // Should not be greater than 65535 (i.e., 2 bytes).
  *ptr = 0; ptr++;
  *ptr = 0; ptr++;
  *ptr = (ICMP_HDRLEN + payloadlen) / 256;
  ptr++;
  *ptr = (ICMP_HDRLEN + payloadlen) % 256;
  ptr++;
  chksumlen += 4;

  // Copy zero field to buf (24 bits)
  *ptr = 0; ptr++;
  *ptr = 0; ptr++;
  *ptr = 0; ptr++;
  chksumlen += 3;

  // Copy next header field to buf (8 bits)
  memcpy (ptr, &iphdr.ip6_nxt, sizeof (iphdr.ip6_nxt));
  ptr += sizeof (iphdr.ip6_nxt);
  chksumlen += sizeof (iphdr.ip6_nxt);

  // Copy ICMPv6 type to buf (8 bits)
  memcpy (ptr, &icmp6hdr.icmp6_type, sizeof (icmp6hdr.icmp6_type));
  ptr += sizeof (icmp6hdr.icmp6_type);
  chksumlen += sizeof (icmp6hdr.icmp6_type);

  // Copy ICMPv6 code to buf (8 bits)
  memcpy (ptr, &icmp6hdr.icmp6_code, sizeof (icmp6hdr.icmp6_code));
  ptr += sizeof (icmp6hdr.icmp6_code);
  chksumlen += sizeof (icmp6hdr.icmp6_code);

  // Copy ICMPv6 ID to buf (16 bits)
  memcpy (ptr, &icmp6hdr.icmp6_id, sizeof (icmp6hdr.icmp6_id));
  ptr += sizeof (icmp6hdr.icmp6_id);
  chksumlen += sizeof (icmp6hdr.icmp6_id);

  // Copy ICMPv6 sequence number to buff (16 bits)
  memcpy (ptr, &icmp6hdr.icmp6_seq, sizeof (icmp6hdr.icmp6_seq));
  ptr += sizeof (icmp6hdr.icmp6_seq);
  chksumlen += sizeof (icmp6hdr.icmp6_seq);

  // Copy ICMPv6 checksum to buf (16 bits)
  // Zero, since we don't know it yet.
  *ptr = 0; ptr++;
  *ptr = 0; ptr++;
  chksumlen += 2;

  // Copy ICMPv6 payload to buf
  memcpy (ptr, payload, payloadlen);
  ptr += payloadlen;
  chksumlen += payloadlen;

  // Pad to the next 16-bit boundary
  for (i=0; i<payloadlen%2; i++, ptr++) {
    *ptr = 0;
    ptr += 1;
    chksumlen += 1;
  }

  return checksum ((unsigned short int *) buf, chksumlen);
}
