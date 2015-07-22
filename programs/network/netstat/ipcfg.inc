;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;                                                                 ;;
;; Copyright (C) KolibriOS team 2010-2015. All rights reserved.    ;;
;; Distributed under terms of the GNU General Public License       ;;
;;                                                                 ;;
;;  Written by hidnplayr@kolibrios.org                             ;;
;;                                                                 ;;
;;          GNU GENERAL PUBLIC LICENSE                             ;;
;;             Version 2, June 1991                                ;;
;;                                                                 ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

enum_callback:

ipcfg:

        mcall   40, EVM_MOUSE + EVM_MOUSE_FILTER + EVM_REDRAW + EVM_BUTTON + EVM_KEY + EVM_STACK

; Create the 'ip..' string with correct device number
        call    create_str_ini_int

; Try to read settings from .ini file
        invoke  ini.get_str, net_ini, str_ini_int, str_ini_ip_type, ini_buf, 16, sz_0
        test    eax, eax
        jz      @f
; If settings not found, use default settings from 'ip?' section
        mov     dword[str_ini_int], 'ip?'
  @@:

; Read ip/subnet/gateway type
        mov     [option_ip], op_ip_static
        invoke  ini.get_str, net_ini, str_ini_int, str_ini_ip_type, ini_buf, 16, sz_0
;        test    eax, eax
;        jz      .disabled
        mov     eax, dword[ini_buf]
        or      eax, 0x20202020
        cmp     eax, 'stat'
        je      @f
        mov     [option_ip], op_ip_auto
  @@:

; Read DNS type
        mov     [option_dns], op_dns_static
        invoke  ini.get_str, net_ini, str_ini_int, str_ini_dns_type, ini_buf, 16, sz_0
        mov     eax, dword[ini_buf]
        or      eax, 0x20202020
        cmp     eax, 'stat'
        je      @f
        mov     [option_dns], op_dns_auto
  @@:

; Read all IP addresses
        mov     [str_edit_ip], 0
        mov     [str_edit_subnet], 0
        mov     [str_edit_gateway], 0
        mov     [str_edit_dns], 0

        invoke  ini.get_str, net_ini, str_ini_int, str_ini_ip, str_edit_ip, 16, sz_0
        invoke  ini.get_str, net_ini, str_ini_int, str_ini_subnet, str_edit_subnet, 16, sz_0
        invoke  ini.get_str, net_ini, str_ini_int, str_ini_gateway, str_edit_gateway, 16, sz_0
        invoke  ini.get_str, net_ini, str_ini_int, str_ini_dns, str_edit_dns, 16, sz_0

        invoke  edit_set_text, edit_ip, str_edit_ip
        invoke  edit_set_text, edit_subnet, str_edit_subnet
        invoke  edit_set_text, edit_gateway, str_edit_gateway
        invoke  edit_set_text, edit_dns, str_edit_dns

  .parse_settings:
        call    toggle_editboxes

  .draw:
; get system colors
        mcall   48, 3, sc, 40

        mcall   12, 1

        mov     edx, [sc.work]
        or      edx, 0x34000000
        xor     esi, esi
        mov     edi, str_title
        mcall   0, 50 shl 16 + 200, 30 shl 16 + 200

        mov     ecx, 0x80000000
        or      ecx, [sc.work_text]
;        mov     ebx, 47 shl 16 + 10
;        mov     edx, str_mode
;        mcall
        mov     ebx, 59 shl 16 + 35
        mov     edx, sz_ip
        mcall   4
        mov     ebx, 5 shl 16 + 55
        mov     edx, sz_subnet
        mcall
        mov     ebx, 29 shl 16 + 75
        mov     edx, sz_gateway
        mcall
;        mov     ebx, 47 shl 16 + 80
;        mov     edx, str_mode
;        mcall
        mov     ebx, 53 shl 16 + 120
        mov     edx, sz_dns
        mcall

        invoke  edit_box_draw, edit_ip
        invoke  edit_box_draw, edit_subnet
        invoke  edit_box_draw, edit_gateway
        invoke  edit_box_draw, edit_dns

        invoke  option_box_draw, Option_boxs1
        invoke  option_box_draw, Option_boxs2

        mcall   8, 80 shl 16 + 100, 150 shl 16 + 16, 2, [sc.work_button]

        mov     ecx, 0x80000000
        or      ecx, [sc.work_button_text]
        mov     ebx, 112 shl 16 + 155
        mov     edx, sz_apply
        mcall   4

        mcall   12, 2

  .loop:
        mcall   10
        dec     eax
        jz      .draw
        dec     eax
        jz      .key
        dec     eax
        jz      .btn

        cmp     [edit_ip.color], 0xcacaca
        je      @f
        invoke  edit_box_mouse, edit_ip
        invoke  edit_box_mouse, edit_subnet
        invoke  edit_box_mouse, edit_gateway
  @@:

        cmp     [edit_dns.color], 0xcacaca
        je      @f
        invoke  edit_box_mouse, edit_dns
  @@:

        push    [option_ip] [option_dns]
        invoke  option_box_mouse, Option_boxs1
        invoke  option_box_mouse, Option_boxs2
        pop     ebx eax
        cmp     eax, [option_ip]
        jne     @f
        cmp     ebx, [option_dns]
        je      .loop
  @@:
        call    toggle_editboxes
        jmp     .draw

  .btn:
        mcall   17

        test    ah , ah
        jz      .loop

        cmp     ah, 2   ; apply
        jne     .exit

; Apply settings!
  .apply:
        xor     edi, edi

        cmp     [edit_ip.color], 0xcacaca
        je      .skip_ip
        mov     [edit_ip.color], 0xffffff
        mov     esi, str_edit_ip
        call    validate_ip
        test    eax, eax
        jz      @f
        mov     [edit_ip.color], 0xff4444
        inc     edi
  @@:

        mov     [edit_subnet.color], 0xffffff
        mov     esi, str_edit_subnet
        call    validate_ip
        test    eax, eax
        jz      @f
        mov     [edit_subnet.color], 0xff4444
        inc     edi
  @@:

        mov     [edit_gateway.color], 0xffffff
        mov     esi, str_edit_gateway
        call    validate_ip
        test    eax, eax
        jz      @f
        mov     [edit_gateway.color], 0xff4444
        inc     edi
  @@:
  .skip_ip:

        cmp     [edit_dns.color], 0xcacaca
        je      .skip_dns
        mov     [edit_dns.color], 0xffffff
        mov     esi, str_edit_dns
        call    validate_ip
        test    eax, eax
        jz      @f
        mov     [edit_dns.color], 0xff4444
        inc     edi
  @@:
  .skip_dns:

        test    edi, edi
        jnz     .draw

; Settings look valid, save them to the ini file
; Re-create 'ip..' string, we dont want to save to 'ip?' section.
        call    create_str_ini_int

        cmp     [option_ip], op_ip_auto
        jne     .ip_static
        invoke  ini.set_str, net_ini, str_ini_int, str_ini_ip_type, sz_dhcp, 4
        jmp     @f
  .ip_static:
        invoke  ini.set_str, net_ini, str_ini_int, str_ini_ip_type, sz_static2, 6
  @@:

        cmp     [option_dns], op_dns_auto
        invoke  ini.set_str, net_ini, str_ini_int, str_ini_dns_type, sz_dhcp, 4
        jmp     @f
  .dns_static:
        invoke  ini.set_str, net_ini, str_ini_int, str_ini_dns_type, sz_static2, 6
  @@:

        invoke  ini.set_str, net_ini, str_ini_int, str_ini_ip, str_edit_ip, [edit_ip.size]
        invoke  ini.set_str, net_ini, str_ini_int, str_ini_subnet, str_edit_subnet, [edit_subnet.size]
        invoke  ini.set_str, net_ini, str_ini_int, str_ini_gateway, str_edit_gateway, [edit_gateway.size]
        invoke  ini.set_str, net_ini, str_ini_int, str_ini_dns, str_edit_dns, [edit_dns.size]

  .exit:
        mcall   -1

  .key:
        mcall   2

; Stupid editbox doesnt allow us to disable edit boxes nor filter input decently, so we do it here.
        cmp     ah, 13
        je      .apply
        cmp     ah, 8
        je      @f
        cmp     ah, '.'
        je      @f
        cmp     ah, '0'
        jb      .loop
        cmp     ah, '9'
        ja      .loop
  @@:
        cmp     [edit_ip.color], 0xffffff
        jne     @f
        invoke  edit_box_key, edit_ip
        invoke  edit_box_key, edit_subnet
        invoke  edit_box_key, edit_gateway
  @@:

        cmp     [edit_dns.color], 0xffffff
        jne     @f
        invoke  edit_box_key, edit_dns
  @@:

        jmp     .loop

toggle_editboxes:

        mov     [edit_ip.color], 0xffffff
        mov     [edit_subnet.color], 0xffffff
        mov     [edit_gateway.color], 0xffffff
        cmp     [option_ip], op_ip_static
        je      @f
        mov     [edit_ip.color], 0xcacaca
        mov     [edit_subnet.color], 0xcacaca
        mov     [edit_gateway.color], 0xcacaca
  @@:

        mov     [edit_dns.color], 0xffffff
        cmp     [option_dns], op_dns_static
        je      @f
        mov     [edit_dns.color], 0xcacaca
  @@:

        ret


; in:   esi = ptr to ascii ip
; out:  eax = 0 - ok / -1 - error
validate_ip:
        xor     ecx, ecx        ; octet count
  .outer_loop:
        xor     ebx, ebx        ; current number
        inc     ecx
  .inner_loop:
        lodsb                   ; load one character from ascii string
        test    al, al
        je      .finish
        cmp     al, '.'
        je      .outer_loop
        ; Convert current digit from ascii to bin, if valid
        sub     al, '0'
        jb      .invalid
        cmp     al, 9
        ja      .invalid
        ; multiply current number by 10
        add     bl, bl
        jc      .invalid
        lea     ebx, [ebx + 4 * ebx]
        test    ebx, 0xffffff00
        jnz     .invalid
        ; add current digit to current number
        add     bl, al
        jc      .invalid
        jmp     .inner_loop
  .finish:
        cmp     cl, 4
        jne     .invalid
        xor     eax, eax
        ret

  .invalid:
        or      eax, -1
        ret

create_str_ini_int:
        movzx   eax, [device]
        mov     ebx, 10
        xor     edx, edx
        push    0
  @@:
        div     ebx
        add     dl, '0'
        push    edx
        test    eax, eax
        jnz     @r
  @@:
        mov     edi, str_ini_int+2
  @@:
        pop     eax
        stosb
        test    eax, eax
        jnz     @r

        ret


;-------------------------
; DATA

edit_ip         edit_box 100, 80, 30, 0xffffff, 0x6f9480, 0, 0, 0, 15, str_edit_ip, mouse_dd, 0, 0, 1
edit_subnet     edit_box 100, 80, 50, 0xffffff, 0x6f9480, 0, 0, 0, 15, str_edit_subnet, mouse_dd, 0, 0, 1
edit_gateway    edit_box 100, 80, 70, 0xffffff, 0x6f9480, 0, 0, 0, 15, str_edit_gateway, mouse_dd, 0, 0, 1

edit_dns        edit_box 100, 80, 115, 0xffffff, 0x6f9480, 0, 0, 0, 15, str_edit_dns, mouse_dd, 0, 0, 1

op_ip_auto      option_box option_ip, 15, 10, 6, 12, 0xffffff, 0, 0, sz_auto, 5
op_ip_static    option_box option_ip, 80, 10, 6, 12, 0xffffff, 0, 0, sz_static, 7

op_dns_auto     option_box option_dns, 15, 95, 6, 12, 0xffffff, 0, 0, sz_auto, 5
op_dns_static   option_box option_dns, 80, 95, 6, 12, 0xffffff, 0, 0, sz_static, 7

option_ip       dd op_ip_auto
option_dns      dd op_dns_auto
Option_boxs1    dd op_ip_auto, op_ip_static, 0
Option_boxs2    dd op_dns_auto, op_dns_static, 0

net_ini         db '/sys/settings/network.ini', 0

str_ini_int     db 'ip1', 0
                rb 10
str_ini_ip      db 'ip', 0
str_ini_subnet  db 'subnet', 0
str_ini_gateway db 'gateway', 0
str_ini_dns     db 'dns1', 0
str_ini_ip_type db 'ip_type', 0
str_ini_dns_type db 'dns_type', 0

sz_0            db 0
sz_dhcp         db 'dhcp', 0
sz_static2      db 'static', 0

str_title       db 'IP settings', 0
sz_ip           db 'IP:', 0
sz_subnet       db 'Subnet mask:', 0
sz_gateway      db 'Gateway:', 0
sz_dns          db 'DNS:', 0
sz_auto         db 'Auto', 0
sz_static       db 'Static', 0
sz_apply        db 'Apply', 0

str_edit_ip     rb 16
str_edit_subnet rb 16
str_edit_gateway rb 16
str_edit_dns    rb 16

ini_buf         rb 16

mouse_dd        dd ?