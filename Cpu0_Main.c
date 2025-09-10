#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "IfxStm.h"
#include "IfxGeth_Eth.h"
#include "Ifx_Lwip.h"
#include "IfxPort.h"
#include "lwip/netif.h"
#include "lwip/ip_addr.h"
#include "lwip/etharp.h"
#include "lwip/udp.h"
#include "bridge_common.h"
#include "can_hal.h"
#include "Configuration.h"
#include "ConfigurationIsr.h"
#include <string.h>
#include "Bsp.h"           // wait() için
#include "lwip/etharp.h"   // ARP fonksiyonları için
#include "lwip/pbuf.h"     // pbuf için
#include"lwipopts.h"
#include "netif/etharp.h"
IfxCpu_syncEvent g_cpuSyncEvent = 0;
volatile uint32_t udp_rx_count = 0;
volatile uint32_t can_tx_count = 0;
struct udp_pcb *g_udp_rx_pcb = NULL;

void udp_recv_callback(void *arg, struct udp_pcb *pcb, struct pbuf *p,
                      const ip_addr_t *addr, u16_t port)
{
    if(!p) return;

    udp_rx_count++;
    IfxPort_togglePin(&MODULE_P00, 5);  // LED toggle

    if(p->tot_len >= sizeof(CanUdpFrame)) {
        CanUdpFrame udp_frame;
        pbuf_copy_partial(p, &udp_frame, sizeof(CanUdpFrame), 0);

        // UDP to CAN
        CanFrame can_frame;
        can_frame.id = lwip_ntohl(udp_frame.id_be);
        can_frame.ide = udp_frame.ide;
        can_frame.rtr = udp_frame.rtr;
        can_frame.dlc = udp_frame.dlc;
        memcpy(can_frame.data, udp_frame.data, 8);

        if(can_hal_transmit(&can_frame, 10)) {
            can_tx_count++;
            IfxPort_togglePin(&MODULE_P00, 6);
        }
    }
    pbuf_free(p);
}

IFX_INTERRUPT(updateLwIPStackISR, 0, ISR_PRIORITY_OS_TICK)
{
    IfxStm_increaseCompare(&MODULE_STM0, IfxStm_Comparator_0, IFX_CFG_STM_TICKS_PER_MS);
    g_TickCount_1ms++;
    Ifx_Lwip_onTimerTick();
}

void core0_main(void)
{
    IfxCpu_enableInterrupts();
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());

    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 1);

    // LED Setup
    IfxPort_setPinModeOutput(&MODULE_P00, 5, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(&MODULE_P00, 6, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);

    // STM timer
    IfxStm_CompareConfig stmConfig;
    IfxStm_initCompareConfig(&stmConfig);
    stmConfig.triggerPriority = ISR_PRIORITY_OS_TICK;
    stmConfig.comparatorInterrupt = IfxStm_ComparatorInterrupt_ir0;
    stmConfig.ticks = IFX_CFG_STM_TICKS_PER_MS;
    stmConfig.typeOfService = IfxSrc_Tos_cpu0;
    IfxStm_initCompare(&MODULE_STM0, &stmConfig);

    // GETH Enable
    IfxGeth_enableModule(&MODULE_GETH);

    // MAC & IP Setup
    eth_addr_t ethAddr;
    MAC_ADDR(&ethAddr, 0x00, 0x11, 0x22, 0x33, 0x44, 0x20);
    Ifx_Lwip_init(ethAddr);

    struct netif *netif = Ifx_Lwip_getNetIf();
    ip_addr_t ip, mask, gw;
    IP_ADDR4(&ip, 192, 168, 10, 20);
    IP_ADDR4(&mask, 255, 255, 255, 0);
    IP_ADDR4(&gw, 192, 168, 10, 1);
    netif_set_addr(netif, &ip, &mask, &gw);
    netif_set_up(netif);
    netif_set_link_up(netif);

    // CAN init
    can_hal_init(500000);

    // UDP receiver init
    g_udp_rx_pcb = udp_new();
    udp_bind(g_udp_rx_pcb, IP_ADDR_ANY, BRIDGE_UDP_PORT);
    udp_recv(g_udp_rx_pcb, udp_recv_callback, NULL);

    // ARP table'a Kart A'yı manuel ekle
    ip_addr_t karta_ip;
    struct eth_addr karta_mac;

    IP_ADDR4(&karta_ip, 192,168,10,10);
    karta_mac.addr[0]=0x00; karta_mac.addr[1]=0x11;
    karta_mac.addr[2]=0x22; karta_mac.addr[3]=0x33;
    karta_mac.addr[4]=0x44; karta_mac.addr[5]=0x10;


    etharp_query(netif, ip_2_ip4(&karta_ip), NULL);



    uint32_t arp_timer = 0;

    while(1) {
        Ifx_Lwip_pollTimerFlags();
        Ifx_Lwip_pollReceiveFlags();

        // ARP refresh
        if(++arp_timer > 5000) {
            arp_timer = 0;
            etharp_gratuitous(netif);
        }

        wait(1);
    }
}
