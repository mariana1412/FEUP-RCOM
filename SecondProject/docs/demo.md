# RCOM DEMO

## Reset TUXs

Reboot them.

Once rebooted, login with username `root` and password `bill9gates`.

While they are rebooting, move on to the next step.

## Wiring

Rack | Switch Port
:---:|:---:
tux2 eth0 | switch 2
tux3 eth0 | switch 3
tux4 eth0 | switch 4
tux4 eth1 | switch 5
router GE0/0 | switch 6

Connect **router GE0/1** to the **1.1** port, in the upper left corner.

Connect **tux3** to the **T3** port, in the upper right corner.

Connect **T4** to **Switch Console** or **Router Console**, as needed.

## Reset configurations

### SWITCH

    configure terminal
    no vlan 2-4096
    exit
    copy flash:tux1-clean startup-config
    reload

### ROUTER

    copy flash:tux1-clean startup-config
    reload

## Configuring the Switch

1. Open GTKTerm in tux3
2. Press enter a few times, so that the prompt appears
3. Run these commands:
    ```
    enable
    (password) 8nortel
    configure terminal
    vlan 10
    vlan 11
    interface fastethernet 0/2
    switchport mode access
    switchport access vlan 11
    exit
    interface fastethernet 0/3
    switchport mode access
    switchport access vlan 10
    exit
    interface fastethernet 0/4
    switchport mode access
    switchport access vlan 10
    exit
    interface fastethernet 0/5
    switchport mode access
    switchport access vlan 11
    exit
    interface fastethernet 0/6
    switchport mode access
    switchport access vlan 11
    exit
    end
    show vlan
    ```
4. Expected results:

    vlan | switchports
    :---:|:---:
    10 | 3, 4
    11 | 2, 5, 6

## Configuring the Router

1. Connect **T4** to **Router Console**
2. Open GTKTerm in tux3
3. Login:
    ```
    username: root
    password: 8nortel
    ```
4. Run these commands:
    ```
    configure terminal

    interface gigabitethernet 0/0
    ip address 172.16.11.254 255.255.255.0
    no shutdown
    ip nat inside
    exit

    interface gigabitethernet 0/1
    ip address 172.16.1.19 255.255.255.0
    no shutdown
    ip nat outside
    exit

    ip nat pool ovrld 172.16.1.19 172.16.1.19 prefix 24
    ip nat inside source list 1 pool ovrld overload

    access-list 1 permit 172.16.10.0 0.0.0.7
    access-list 1 permit 172.16.11.0 0.0.0.7

    ip route 0.0.0.0 0.0.0.0 172.16.1.254
    ip route 172.16.10.0 255.255.255.0 172.16.11.253
    
    end
    ```

## Configuring TUXs

Run these commands in each TUX terminal:

### TUX2

```
ifconfig eth0 up
ifconfig eth0 172.16.11.1/24
route add -net 172.16.10/24 gw 172.16.11.253
route add default gw 172.16.11.254
echo $'search netlab.fe.up.pt\nnameserver 172.16.1.1' > /etc/resolv.conf
```

### TUX3

```
ifconfig eth0 up
ifconfig eth0 172.16.10.1/24
route add -net 172.16.11.1/24 gw 172.16.10.254
route default gw 172.16.10.254
echo $'search netlab.fe.up.pt\nnameserver 172.16.1.1' > /etc/resolv.conf
```

### TUX4

```
ifconfig eth0 up
ifconfig eth0 172.16.10.254/24
ifconfig eth1 up
ifconfig eth1 172.16.11.253/24
route add default gw 172.16.11.254
echo 1 > /proc/sys/net/ipv4/ip_forward
echo 0 > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts
```

## Test


### TUX2

```
ping 172.16.11.253  #tux4 vlan 11
ping 172.16.10.1    #tux3 vlan 10
ping 172.16.11.253  #router
ping 172.16.19      #netlab
ping 8.8.8.8        #internet
ping google.com     #internet with DNS
```

### TUX3

```
ping 172.16.10.254  #tux4 vlan 10
ping 172.16.11.1    #tux2 vlan 11
ping 172.16.11.253  #router
ping 172.16.19      #netlab
ping 8.8.8.8        #internet
ping google.com     #internet with DNS
```

### TUX4

```
ping 172.16.10.1    #tux3 vlan 10
ping 172.16.11.1    #tux2 vlan 11
ping 172.16.11.254  #router
ping 172.16.1.19    #netlab (not supposed to work)
```
