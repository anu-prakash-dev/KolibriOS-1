; Zero-config
; v 1.4
;
; DHCP code is based on that by Mike Hibbet (DHCP client for menuetos)
;
; Written by HidnPlayr & Derpenguin

format binary as ""

use32
               org    0x0

               db     'MENUET01'            ; 8 byte id
               dd     0x01                  ; header version
               dd     START                 ; start of code
               dd     IM_END                ; size of image
               dd     (I_END+0x100)         ; memory for app
               dd     (I_END+0x100)         ; esp
               dd     0x0 , path            ; I_Param , I_Icon

; CONFIGURATION


TIMEOUT             equ 60                  ; in seconds
BUFFER              equ 1024                ; in bytes
__DEBUG__           equ 1                   ; enable/disable
__DEBUG_LEVEL__     equ 1                   ; 1 = all, 2 = errors

; CONFIGURATION FOR LINK-LOCAL

PROBE_WAIT          equ 1                   ; second  (initial random delay)
PROBE_MIN           equ 1                   ; second  (minimum delay till repeated probe)
PROBE_MAX           equ 2                   ; seconds (maximum delay till repeated probe)
PROBE_NUM           equ 3                   ;         (number of probe packets)

ANNOUNCE_NUM        equ 2                   ;         (number of announcement packets)
ANNOUNCE_INTERVAL   equ 2                   ; seconds (time between announcement packets)
ANNOUNCE_WAIT       equ 2                   ; seconds (delay before announcing)

MAX_CONFLICTS       equ 10                  ;         (max conflicts before rate limiting)

RATE_LIMIT_INTERVAL equ 60                  ; seconds (delay between successive attempts)

DEFEND_INTERVAL     equ 10                  ; seconds (min. wait between defensive ARPs)


include '../proc32.inc'
include '../macros.inc'
include '../debug-fdo.inc'
include '../network.inc'
include 'dhcp.inc'
include '../dll.inc'


Ip2dword:
    push    edx

    ; This code validates if the query is an IP containing 4 numbers and 3 dots

    xor     al, al            ; make al (dot count) zero

   @@:
    cmp     byte[edx],'0'     ; check if this byte is a number, if not jump to no_IP
    jl      no_IP             ;
    cmp     byte[edx],'9'     ;
    jg      no_IP             ;

    inc     edx               ; the byte was a number, so lets check the next byte

    cmp     byte[edx],0       ; is this byte zero? (have we reached end of query?)
    jz      @f                ; jump to next @@ then
    cmp     byte[edx],':'
    jz      @f

    cmp     byte[edx],'.'     ; is this byte a dot?
    jne     @r                ; if not, jump to previous @@

    inc     al                ; the byte was a dot so increment al(dot count)
    inc     edx               ; next byte
    jmp     @r                ; lets check for numbers again (jump to previous @@)

   @@:                        ; we reach this when end of query reached
    cmp     al,3              ; check if there where 3 dots
    jnz     no_IP             ; if not, jump to no_IP

    ; The following code will convert this IP into a dword and output it in eax
    ; If there is also a port number specified, this will be returned in ebx, otherwise ebx is -1

    pop     esi               ; edx (query address) was pushed onto stack and is now popped in esi

    xor     edx, edx          ; result
    xor     eax, eax          ; current character
    xor     ebx, ebx          ; current byte

.outer_loop:
    shl     edx, 8
    add     edx, ebx
    xor     ebx, ebx
.inner_loop:
    lodsb
    test    eax, eax
    jz      .finish
    cmp     al, '.'
    jz      .outer_loop
    sub     eax, '0'
    imul    ebx, 10
    add     ebx, eax
    jmp     .inner_loop
.finish:
    shl     edx, 8
    add     edx, ebx

    bswap   edx               ; we want little endian order

    ret

no_IP:
    pop     edx
    xor     edx, edx

    ret






START:                                      ; start of execution

        mcall   40, 1 shl 7 ; network event

        DEBUGF  1,">Zero-config service:\n"

        mcall   76, API_ETH + 4

        cmp     eax, -1
        je      exit

        mov     word[MAC], bx
        mov     dword[MAC+2], eax

        DEBUGF  1,"->MAC: %x-%x-%x-%x-%x-%x\n",[MAC]:2,[MAC+1]:2,[MAC+2]:2,[MAC+3]:2,[MAC+4]:2,[MAC+5]:2

        cld
        mov     edi, path      ; Calculate the length of zero-terminated string
        xor     al , al
        mov     ecx, 1024
        repnz   scas byte[es:edi]
        dec     edi

        mov     esi, filename
        movsd
        movsb

        DEBUGF  1,"->path to ini: %s\n", path

        mcall   68,11

        stdcall dll.Load,@IMPORT
        or      eax,eax
        jnz     skip_ini


        invoke ini.get_str, path, str_ipconfig, str_type, inibuf, 16, 0

        mov    eax,dword[inibuf]

        cmp    eax,'stat'
        jne    skip_ini

        invoke ini.get_str, path, str_ipconfig, str_ip, inibuf, 16, 0
        mov    edx, inibuf
        call   Ip2dword
        mcall  76, API_IPv4 + 3, edx

        invoke ini.get_str, path, str_ipconfig, str_gateway, inibuf, 16, 0
        mov    edx, inibuf
        call   Ip2dword
        mcall  76, API_IPv4 + 9, edx

        invoke ini.get_str, path, str_ipconfig, str_dns, inibuf, 16, 0
        mov    edx, inibuf
        call   Ip2dword
        mcall  76, API_IPv4 + 5, edx

        invoke ini.get_str, path, str_ipconfig, str_subnet, inibuf, 16, 0
        mov    edx, inibuf
        call   Ip2dword
        mcall  76, API_IPv4 + 7, edx


        mcall  -1


skip_ini:

        DEBUGF  1,"->Skip ini\n"

        mcall 75, 0, AF_INET4, SOCK_DGRAM, 0      ; open socket (parameters: domain, type, reserved)
        cmp   eax, -1
        je    error
        mov   [socketNum], eax

        DEBUGF  1,"->socket %x opened\n", eax

        mcall 75, 2, [socketNum], sockaddr1, 18     ; bind socket to local port 68
        cmp   eax, -1
        je    error

        DEBUGF  1,"->Socket Bound to local port 68\n"

        mcall 75, 4, [socketNum], sockaddr2, 18     ; connect to 255.255.255.255 on port 67
        cmp   eax, -1
        je    error

        DEBUGF  1,"->Connected to 255.255.255.255 on port 67\n"

        mov     byte [dhcpMsgType], 0x01        ; DHCP discover
        mov     dword [dhcpLease], esi          ; esi is still -1 (-1 = forever)

        mcall   26, 9
        imul    eax,100
        mov     [currTime],eax

buildRequest:                               ; Creates a DHCP request packet.

        DEBUGF  1,"->Building request\n"

        stdcall mem.Alloc, BUFFER
        mov     [dhcpMsg], eax
        test    eax,eax
        jz      apipa

            ;;; todo: skip this bullcrap

        mov     edi, eax
        mov     ecx, BUFFER
        xor     eax, eax
        cld
        rep     stosb

            ;; todo: put this in a buffer instead of writing bytes and words!

        mov     edx,[dhcpMsg]

        mov     [edx], byte 0x01                ; Boot request
        mov     [edx+1], byte 0x01              ; Ethernet
        mov     [edx+2], byte 0x06              ; Ethernet h/w len
        mov     [edx+4], dword 0x11223344       ; xid                 ;;;;;;;
        mov     eax,[currTime]
        mov     [edx+8], eax                    ; secs, our uptime
        mov     [edx+10], byte 0x80             ; broadcast flag set
        mov     eax, dword [MAC]                ; first 4 bytes of MAC
        mov     [edx+28],dword eax
        mov     ax, word [MAC+4]                ; last 2 bytes of MAC
        mov     [edx+32],word ax
        mov     [edx+236], dword 0x63538263     ; magic cookie
        mov     [edx+240], word 0x0135          ; option DHCP msg type
        mov     al, [dhcpMsgType]
        mov     [edx+240+2], al
        mov     [edx+240+3], word 0x0433        ; option Lease time = infinity
        mov     eax, [dhcpLease]
        mov     [edx+240+5], eax
        mov     [edx+240+9], word 0x0432        ; option requested IP address
        mov     eax, [dhcp.ip]
        mov     [edx+240+11], eax
        mov     [edx+240+15], word 0x0437       ; option request list
        mov     [edx+240+17], dword 0x0f060301

        cmp     [dhcpMsgType], byte 0x01        ; Check which msg we are sending
        jne     request_options

        mov     [edx+240+21], byte 0xff         ; "Discover" options

        mov     [dhcpMsgLen], dword 262         ; end of options marker
        jmp     send_request

request_options:
        mov     [edx+240+21], word 0x0436       ; server IP
        mov     eax, [dhcpServerIP]
        mov     [edx+240+23], eax

        mov     [edx+240+27], byte 0xff         ; end of options marker

        mov     [dhcpMsgLen], dword 268

send_request:
        mcall   75, 6, [socketNum], [dhcpMsg], [dhcpMsgLen]     ; write to socket ( send broadcast request )

        mov     eax, [dhcpMsg]                          ; Setup the DHCP buffer to receive response
        mov     [dhcpMsgLen], eax                       ; Used as a pointer to the data

        mcall   23, TIMEOUT*10                          ; wait for data

read_data:                                              ; we have data - this will be the response
        mcall   75, 7, [socketNum], [dhcpMsg], BUFFER   ; read data from socket

        DEBUGF  1,"->%d bytes received\n", eax

        cmp     eax, -1
        je      error

        mov     [dhcpMsgLen], eax

    ; depending on which msg we sent, handle the response
    ; accordingly.
    ; If the response is to a dhcp discover, then:
    ;  1) If response is DHCP OFFER then
    ;  1.1) record server IP, lease time & IP address.
    ;  1.2) send a request packet
    ; If the response is to a dhcp request, then:
    ;  1) If the response is DHCP ACK then
    ;  1.1) extract the DNS & subnet fields. Set them in the stack

    cmp     [dhcpMsgType], byte 0x01        ; did we send a discover?
    je      discover
    cmp     [dhcpMsgType], byte 0x03        ; did we send a request?
    je      request

        jmp     exit                           ; really unknown, what we did

discover:
        call    parseResponse

        cmp     [dhcpMsgType], byte 0x02        ; Was the response an offer?
        jne     apipa                           ; NO - so we do zeroconf
        mov     [dhcpMsgType], byte 0x03        ; DHCP request
        jmp     buildRequest

request:
        call    parseResponse

        cmp     [dhcpMsgType], byte 0x05        ; Was the response an ACK? It should be
        jne     apipa                           ; NO - so we do zeroconf

        mcall 76, API_IPv4 + 3, [dhcp.ip]       ; ip
        mcall 76, API_IPv4 + 5, [dhcp.dns]      ; dns
        mcall 76, API_IPv4 + 7, [dhcp.subnet]   ; subnet
        mcall 76, API_IPv4 + 9, [dhcp.gateway]  ; gateway

        jmp     exit

;***************************************************************************
;   Function
;      parseResponse
;
;   Description
;      extracts the fields ( client IP address and options ) from
;      a DHCP response
;      The values go into
;       dhcpMsgType,dhcpLease,dhcpClientIP,dhcpServerIP,
;       dhcpDNSIP, dhcpSubnet
;      The message is stored in dhcpMsg
;
;***************************************************************************
parseResponse:
    DEBUGF  1,"Data received, parsing response\n"
    mov     edx, [dhcpMsg]

    push    dword [edx+16]
    pop     [dhcp.ip]
    DEBUGF  1,"Client: %u.%u.%u.%u\n",[edx+16]:1,[edx+17]:1,[edx+18]:1,[edx+19]:1

    add     edx, 240                        ; Point to first option
    xor     ecx, ecx

next_option:
    add     edx, ecx
pr001:
    mov     al, [edx]
    cmp     al, 0xff                        ; End of options?
    je      pr_exit

    cmp     al, dhcp_msg_type               ; Msg type is a single byte option
    jne     @f

    mov     al, [edx+2]
    mov     [dhcpMsgType], al

    DEBUGF  1,"DHCP Msg type: %u\n", al

    add     edx, 3
    jmp     pr001                           ; Get next option

@@:
    inc     edx
    movzx   ecx, byte [edx]
    inc     edx                             ; point to data

    cmp     al, dhcp_dhcp_server_id         ; server ip
    jne     @f
    mov     eax, [edx]
    mov     [dhcpServerIP], eax
    DEBUGF  1,"Server: %u.%u.%u.%u\n",[edx]:1,[edx+1]:1,[edx+2]:1,[edx+3]:1
    jmp     next_option

@@:
    cmp     al, dhcp_address_time
    jne     @f

    pusha
    mov     eax,[edx]
    bswap   eax
    mov     [dhcpLease],eax
    DEBUGF  1,"lease: %d\n",eax
    popa

    jmp     next_option

@@:
    cmp     al, dhcp_subnet_mask
    jne     @f

    push    dword [edx]
    pop     [dhcp.subnet]
    DEBUGF  1,"Subnet: %u.%u.%u.%u\n",[edx]:1,[edx+1]:1,[edx+2]:1,[edx+3]:1
    jmp     next_option

@@:
    cmp     al, dhcp_router
    jne     @f

    push    dword [edx]
    pop     [dhcp.gateway]
    DEBUGF  1,"Gateway: %u.%u.%u.%u\n",[edx]:1,[edx+1]:1,[edx+2]:1,[edx+3]:1
    jmp     next_option


@@:
    cmp     al, dhcp_domain_server
    jne     next_option

    push    dword [edx]
    pop     [dhcp.dns]
    DEBUGF  1,"DNS: %u.%u.%u.%u\n",[edx]:1,[edx+1]:1,[edx+2]:1,[edx+3]:1
    jmp     next_option

pr_exit:

        ret

;    DEBUGF  1,"Sending ARP announce\n"
;;;


apipa:
        mcall   close, [socketNum]
        stdcall mem.Free, [dhcpMsg]

link_local:
    call random
    mov  ecx,0xfea9                         ; IP 169.254.0.0 link local net, see RFC3927
    mov  cx,ax
    mcall 76, API_IPv4 + 3, ecx                          ; mask is 255.255.0.0
    DEBUGF 1,"Link Local IP assinged: 169.254.%u.%u\n",[generator+2]:1,[generator+3]:1
    mcall 76, API_IPv4 + 5, 0xffff
    mcall 76, API_IPv4 + 9, 0x0
    mcall 76, API_IPv4 + 7, 0x0

    mcall 5, PROBE_WAIT*100

    xor esi,esi
   probe_loop:
    call  random                            ; create a pseudo random number in eax (seeded by MAC)

    cmp   al,PROBE_MIN*100                  ; check if al is bigger then PROBE_MIN
    jge   @f                                ; all ok
    add   al,(PROBE_MAX-PROBE_MIN)*100      ; al is too small
   @@:

    cmp   al,PROBE_MAX*100
    jle   @f
    sub   al,(PROBE_MAX-PROBE_MIN)*100
   @@:

    movzx ebx,al
    DEBUGF  1,"Waiting %u0ms\n",ebx
    mcall 5

    DEBUGF  1,"Sending Probe\n"
;    eth.ARP_PROBE MAC
    inc   esi

    cmp   esi,PROBE_NUM
    jl    probe_loop

; now we wait further ANNOUNCE_WAIT seconds and send ANNOUNCE_NUM ARP announces. If any other host has assingned
; IP within this time, we should create another adress, that have to be done later

    DEBUGF  1,"Waiting %us\n",ANNOUNCE_WAIT
    mcall 5, ANNOUNCE_WAIT*100
    xor   esi,esi
   announce_loop:

    DEBUGF  1,"Sending Announce\n"
;    eth.ARP_ANNOUNCE MAC

    inc   esi
    cmp   esi,ANNOUNCE_NUM
    je    @f

    DEBUGF  1,"Waiting %us\n",ANNOUNCE_INTERVAL
    mcall 5, ANNOUNCE_INTERVAL*100
    jmp   announce_loop
   @@:
    ; we should, instead of closing, detect ARP conflicts and detect if cable keeps connected ;)

error:
exit:
    mcall -1


random:  ; Pseudo random actually

    mov   eax,[generator]
    add   eax,-43ab45b5h
    ror   eax,1
    bswap eax
    xor   eax,dword[MAC]
    ror   eax,1
    xor   eax,dword[MAC+2]
    mov   [generator],eax

ret

; DATA AREA

align 16
@IMPORT:

library \
        libini,'libini.obj'

import  libini, \
        ini.get_str,'ini_get_str'

include_debug_strings

filename db '.ini',0
str_ip db 'ip',0
str_subnet db 'subnet',0
str_gateway db 'gateway',0
str_dns db 'dns',0
str_ipconfig db 'ipconfig',0
str_type db 'type',0


sockaddr1:

        dw AF_INET4
        dw 68 shl 8     ; local port
        dd 0            ; local IP

        rb 10


sockaddr2:

        dw AF_INET4
        dw 67 shl 8     ; destination port
        dd -1           ; destination IP

        rb 10


IM_END:

inibuf          rb 16

dhcpMsgType     db  ?
dhcpLease       dd  ?
dhcpServerIP    dd  ?

dhcp:
.ip             dd  ?
.subnet         dd  ?
.dns            dd  ?
.gateway        dd  ?


dhcpMsgLen      dd  ?
socketNum       dd  ?

MAC             dp  ?

currTime        dd  ?
renewTime       dd  ?
generator       dd  ?

dhcpMsg         dd  ?

I_END_2:

path            rb  1024+5

                rb  65536

I_END: