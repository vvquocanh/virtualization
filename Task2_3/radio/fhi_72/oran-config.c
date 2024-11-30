/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.1  (the "License"); you may not use this file
 * except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.openairinterface.org/?page_id=698
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *-------------------------------------------------------------------------------
 * For more information about the OpenAirInterface (OAI) Software Alliance:
 *      contact@openairinterface.org
 */

#include "oran-config.h"
#include "oran-params.h"
#include "common/utils/assertions.h"
#include "common_lib.h"

#include "xran_fh_o_du.h"
#include "xran_cp_api.h"
#include "rte_ether.h"
#include <rte_ethdev.h>

#include "stdio.h"
#include "string.h"

static void print_fh_eowd_cmn(unsigned index, const struct xran_ecpri_del_meas_cmn *eowd_cmn)
{
  printf("\
    eowd_cmn[%u]:\n\
      initiator_en %d\n\
      numberOfSamples %d\n\
      filterType %d\n\
      responseTo %ld\n\
      measVf %d\n\
      measState %d\n\
      measId %d\n\
      measMethod %d\n\
      owdm_enable %d\n\
      owdm_PlLength %d\n",
      index,
      eowd_cmn->initiator_en,
      eowd_cmn->numberOfSamples,
      eowd_cmn->filterType,
      eowd_cmn->responseTo,
      eowd_cmn->measVf,
      eowd_cmn->measState,
      eowd_cmn->measId,
      eowd_cmn->measMethod,
      eowd_cmn->owdm_enable,
      eowd_cmn->owdm_PlLength);
}

static void print_fh_eowd_port(unsigned index, unsigned vf, const struct xran_ecpri_del_meas_port *eowd_port)
{
  printf("\
    eowd_port[%u][%u]:\n\
      t1 %ld\n\
      t2 %ld\n\
      tr %ld\n\
      delta %ld\n\
      portid %d\n\
      runMeas %d\n\
      currentMeasID %d\n\
      msState %d\n\
      numMeas %d\n\
      txDone %d\n\
      rspTimerIdx %ld\n\
      delaySamples [%ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld]\n\
      delayAvg %ld\n",
      index,
      vf,
      eowd_port->t1,
      eowd_port->t2,
      eowd_port->tr,
      eowd_port->delta,
      eowd_port->portid,
      eowd_port->runMeas,
      eowd_port->currentMeasID,
      eowd_port->msState,
      eowd_port->numMeas,
      eowd_port->txDone,
      eowd_port->rspTimerIdx,
      eowd_port->delaySamples[0],
      eowd_port->delaySamples[1],
      eowd_port->delaySamples[2],
      eowd_port->delaySamples[3],
      eowd_port->delaySamples[4],
      eowd_port->delaySamples[5],
      eowd_port->delaySamples[6],
      eowd_port->delaySamples[7],
      eowd_port->delaySamples[8],
      eowd_port->delaySamples[9],
      eowd_port->delaySamples[10],
      eowd_port->delaySamples[11],
      eowd_port->delaySamples[12],
      eowd_port->delaySamples[13],
      eowd_port->delaySamples[14],
      eowd_port->delaySamples[15],
      eowd_port->delayAvg);
}

static void print_fh_init_io_cfg(const struct xran_io_cfg *io_cfg)
{
  printf("\
  io_cfg:\n\
    id %d (%s)\n\
    num_vfs %d\n\
    num_rxq %d\n\
    dpdk_dev [%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s]\n\
    bbdev_dev %s\n\
    bbdev_mode %d\n\
    dpdkIoVaMode %d\n\
    dpdkMemorySize %d\n",
      io_cfg->id,
      io_cfg->id == 0 ? "O-DU" : "O-RU",
      io_cfg->num_vfs,
      io_cfg->num_rxq,
      io_cfg->dpdk_dev[XRAN_UP_VF],
      io_cfg->dpdk_dev[XRAN_CP_VF],
      io_cfg->dpdk_dev[XRAN_UP_VF1],
      io_cfg->dpdk_dev[XRAN_CP_VF1],
      io_cfg->dpdk_dev[XRAN_UP_VF2],
      io_cfg->dpdk_dev[XRAN_CP_VF2],
      io_cfg->dpdk_dev[XRAN_UP_VF3],
      io_cfg->dpdk_dev[XRAN_CP_VF3],
      io_cfg->dpdk_dev[XRAN_UP_VF4],
      io_cfg->dpdk_dev[XRAN_CP_VF4],
      io_cfg->dpdk_dev[XRAN_UP_VF5],
      io_cfg->dpdk_dev[XRAN_CP_VF5],
      io_cfg->dpdk_dev[XRAN_UP_VF6],
      io_cfg->dpdk_dev[XRAN_CP_VF6],
      io_cfg->dpdk_dev[XRAN_UP_VF7],
      io_cfg->dpdk_dev[XRAN_CP_VF7],
      io_cfg->bbdev_dev[0],
      io_cfg->bbdev_mode,
      io_cfg->dpdkIoVaMode,
      io_cfg->dpdkMemorySize);

  printf("\
    core %d\n\
    system_core %d\n\
    pkt_proc_core %016lx\n\
    pkt_proc_core_64_127 %016lx\n\
    pkt_aux_core %d\n\
    timing_core %d\n\
    port [%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, ]\n\
    io_sleep %d\n\
    nEthLinePerPort %d\n\
    nEthLineSpeed %d\n\
    one_vf_cu_plane %d\n",
      io_cfg->core,
      io_cfg->system_core,
      io_cfg->pkt_proc_core,
      io_cfg->pkt_proc_core_64_127,
      io_cfg->pkt_aux_core,
      io_cfg->timing_core,
      io_cfg->port[XRAN_UP_VF],
      io_cfg->port[XRAN_CP_VF],
      io_cfg->port[XRAN_UP_VF1],
      io_cfg->port[XRAN_CP_VF1],
      io_cfg->port[XRAN_UP_VF2],
      io_cfg->port[XRAN_CP_VF2],
      io_cfg->port[XRAN_UP_VF3],
      io_cfg->port[XRAN_CP_VF3],
      io_cfg->port[XRAN_UP_VF4],
      io_cfg->port[XRAN_CP_VF4],
      io_cfg->port[XRAN_UP_VF5],
      io_cfg->port[XRAN_CP_VF5],
      io_cfg->port[XRAN_UP_VF6],
      io_cfg->port[XRAN_CP_VF6],
      io_cfg->port[XRAN_UP_VF7],
      io_cfg->port[XRAN_CP_VF7],
      io_cfg->io_sleep,
      io_cfg->nEthLinePerPort,
      io_cfg->nEthLineSpeed,
      io_cfg->one_vf_cu_plane);
  print_fh_eowd_cmn(0, &io_cfg->eowd_cmn[0]);
  print_fh_eowd_cmn(1, &io_cfg->eowd_cmn[1]);
  for (int i = 0; i < 2; ++i)
    for (int v = 0; v < io_cfg->num_vfs; ++v)
      print_fh_eowd_port(i, v, &io_cfg->eowd_port[i][v]);
}

static void print_fh_init_eaxcid_conf(const struct xran_eaxcid_config *eaxcid_conf)
{
  printf("\
  eAxCId_conf:\n\
    mask_cuPortId 0x%04x\n\
    mask_bandSectorId 0x%04x\n\
    mask_ccId 0x%04x\n\
    mask_ruPortId 0x%04x\n\
    bit_cuPortId %d\n\
    bit_bandSectorId %d\n\
    bit_ccId %d\n\
    bit_ruPortId %d\n",
      eaxcid_conf->mask_cuPortId,
      eaxcid_conf->mask_bandSectorId,
      eaxcid_conf->mask_ccId,
      eaxcid_conf->mask_ruPortId,
      eaxcid_conf->bit_cuPortId,
      eaxcid_conf->bit_bandSectorId,
      eaxcid_conf->bit_ccId,
      eaxcid_conf->bit_ruPortId);
}

static void print_ether_addr(const char *pre, int num_ether, const struct rte_ether_addr *addrs)
{
  printf("%s [", pre);
  for (int i = 0; i < num_ether; ++i) {
    char buf[18];
    rte_ether_format_addr(buf, 18, &addrs[i]);
    printf("%s", buf);
    if (i != num_ether - 1)
      printf(", ");
  }
  printf("]\n");
}

void print_fh_init(const struct xran_fh_init *fh_init)
{
  printf("xran_fh_init:\n");
  print_fh_init_io_cfg(&fh_init->io_cfg);
  print_fh_init_eaxcid_conf(&fh_init->eAxCId_conf);
  printf("\
  xran_ports %d\n\
  dpdkBasebandFecMode %d\n\
  dpdkBasebandDevice %s\n\
  filePrefix %s\n\
  mtu %d\n\
  p_o_du_addr %s\n",
      fh_init->xran_ports,
      fh_init->dpdkBasebandFecMode,
      fh_init->dpdkBasebandDevice,
      fh_init->filePrefix,
      fh_init->mtu,
      fh_init->p_o_du_addr);
  print_ether_addr("  p_o_ru_addr", fh_init->xran_ports * fh_init->io_cfg.num_vfs, (struct rte_ether_addr *)fh_init->p_o_ru_addr);
  printf("\
  totalBfWeights %d\n",
      fh_init->totalBfWeights);
}

static void print_prach_config(const struct xran_prach_config *prach_conf)
{
  printf("\
  prach_config:\n\
    nPrachConfIdx %d\n\
    nPrachSubcSpacing %d\n\
    nPrachZeroCorrConf %d\n\
    nPrachRestrictSet %d\n\
    nPrachRootSeqIdx %d\n\
    nPrachFreqStart %d\n\
    nPrachFreqOffset %d\n\
    nPrachFilterIdx %d\n\
    startSymId %d\n\
    lastSymId %d\n\
    startPrbc %d\n\
    numPrbc %d\n\
    timeOffset %d\n\
    freqOffset %d\n\
    eAxC_offset %d\n",
      prach_conf->nPrachConfIdx,
      prach_conf->nPrachSubcSpacing,
      prach_conf->nPrachZeroCorrConf,
      prach_conf->nPrachRestrictSet,
      prach_conf->nPrachRootSeqIdx,
      prach_conf->nPrachFreqStart,
      prach_conf->nPrachFreqOffset,
      prach_conf->nPrachFilterIdx,
      prach_conf->startSymId,
      prach_conf->lastSymId,
      prach_conf->startPrbc,
      prach_conf->numPrbc,
      prach_conf->timeOffset,
      prach_conf->freqOffset,
      prach_conf->eAxC_offset);
}

static void print_srs_config(const struct xran_srs_config *srs_conf)
{
  printf("\
  srs_config:\n\
    symbMask %04x\n\
    eAxC_offset %d\n",
      srs_conf->symbMask,
      srs_conf->eAxC_offset);
}

static void print_frame_config(const struct xran_frame_config *frame_conf)
{
  printf("\
  frame_conf:\n\
    nFrameDuplexType %s\n\
    nNumerology %d\n\
    nTddPeriod %d\n",
      frame_conf->nFrameDuplexType == XRAN_TDD ? "TDD" : "FDD",
      frame_conf->nNumerology,
      frame_conf->nTddPeriod);
  for (int i = 0; i < frame_conf->nTddPeriod; ++i) {
    printf("    sSlotConfig[%d]: ", i);
    for (int s = 0; s < XRAN_NUM_OF_SYMBOL_PER_SLOT; ++s) {
      uint8_t nSymbolType = frame_conf->sSlotConfig[i].nSymbolType[s];
      printf("%c", nSymbolType == 0 ? 'D' : (nSymbolType == 1 ? 'U' : 'G'));
    }
    printf("\n");
  }
}

static void print_ru_config(const struct xran_ru_config *ru_conf)
{
  printf("\
  ru_config:\n\
    xranTech %s\n\
    xranCat %s\n\
    xranCompHdrType %s\n\
    iqWidth %d\n\
    compMeth %d\n\
    iqWidth_PRACH %d\n\
    compMeth_PRACH %d\n\
    fftSize %d\n\
    byteOrder %s\n\
    iqOrder %s\n\
    xran_max_frame %d\n",
      ru_conf->xranTech == XRAN_RAN_5GNR ? "NR" : "LTE",
      ru_conf->xranCat == XRAN_CATEGORY_A ? "A" : "B",
      ru_conf->xranCompHdrType == XRAN_COMP_HDR_TYPE_DYNAMIC ? "dynamic" : "static",
      ru_conf->iqWidth,
      ru_conf->compMeth,
      ru_conf->iqWidth_PRACH,
      ru_conf->compMeth_PRACH,
      ru_conf->fftSize,
      ru_conf->byteOrder == XRAN_NE_BE_BYTE_ORDER ? "network/BE" : "CPU/LE",
      ru_conf->iqOrder == XRAN_I_Q_ORDER ? "I_Q" : "Q_I",
      ru_conf->xran_max_frame);
}

void print_fh_config(const struct xran_fh_config *fh_config)
{
  printf("xran_fh_config:\n");
  printf("\
  dpdk_port %d\n\
  sector_id %d\n\
  nCC %d\n\
  neAxc %d\n\
  neAxcUl %d\n\
  nAntElmTRx %d\n\
  nDLFftSize %d\n\
  nULFftSize %d\n\
  nDLRBs %d\n\
  nULRBs %d\n\
  nDLAbsFrePointA %d\n\
  nULAbsFrePointA %d\n\
  nDLCenterFreqARFCN %d\n\
  nULCenterFreqARFCN %d\n\
  ttiCb %p\n\
  ttiCbParam %p\n",
      fh_config->dpdk_port,
      fh_config->sector_id,
      fh_config->nCC,
      fh_config->neAxc,
      fh_config->neAxcUl,
      fh_config->nAntElmTRx,
      fh_config->nDLFftSize,
      fh_config->nULFftSize,
      fh_config->nDLRBs,
      fh_config->nULRBs,
      fh_config->nDLAbsFrePointA,
      fh_config->nULAbsFrePointA,
      fh_config->nDLCenterFreqARFCN,
      fh_config->nULCenterFreqARFCN,
      fh_config->ttiCb,
      fh_config->ttiCbParam);

  printf("\
  Tadv_cp_dl %d\n\
  T2a_min_cp_dl %d\n\
  T2a_max_cp_dl %d\n\
  T2a_min_cp_ul %d\n\
  T2a_max_cp_ul %d\n\
  T2a_min_up %d\n\
  T2a_max_up %d\n\
  Ta3_min %d\n\
  Ta3_max %d\n\
  T1a_min_cp_dl %d\n\
  T1a_max_cp_dl %d\n\
  T1a_min_cp_ul %d\n\
  T1a_max_cp_ul %d\n\
  T1a_min_up %d\n\
  T1a_max_up %d\n\
  Ta4_min %d\n\
  Ta4_max %d\n",
      fh_config->Tadv_cp_dl,
      fh_config->T2a_min_cp_dl,
      fh_config->T2a_max_cp_dl,
      fh_config->T2a_min_cp_ul,
      fh_config->T2a_max_cp_ul,
      fh_config->T2a_min_up,
      fh_config->T2a_max_up,
      fh_config->Ta3_min,
      fh_config->Ta3_max,
      fh_config->T1a_min_cp_dl,
      fh_config->T1a_max_cp_dl,
      fh_config->T1a_min_cp_ul,
      fh_config->T1a_max_cp_ul,
      fh_config->T1a_min_up,
      fh_config->T1a_max_up,
      fh_config->Ta4_min,
      fh_config->Ta4_max);

  printf("\
  enableCP %d\n\
  prachEnable %d\n\
  srsEnable %d\n\
  puschMaskEnable %d\n\
  puschMaskSlot %d\n\
  cp_vlan_tag %d\n\
  up_vlan_tag %d\n\
  debugStop %d\n\
  debugStopCount %d\n\
  DynamicSectionEna %d\n\
  GPS_Alpha %d\n\
  GPS_Beta %d\n",
      fh_config->enableCP,
      fh_config->prachEnable,
      fh_config->srsEnable,
      fh_config->puschMaskEnable,
      fh_config->puschMaskSlot,
      fh_config->cp_vlan_tag,
      fh_config->up_vlan_tag,
      fh_config->debugStop,
      fh_config->debugStopCount,
      fh_config->DynamicSectionEna,
      fh_config->GPS_Alpha,
      fh_config->GPS_Beta);

  print_prach_config(&fh_config->prach_conf);
  print_srs_config(&fh_config->srs_conf);
  print_frame_config(&fh_config->frame_conf);
  print_ru_config(&fh_config->ru_conf);

  printf("\
  bbdev_enc %p\n\
  bbdev_dec %p\n\
  tx_cp_eAxC2Vf %p\n\
  tx_up_eAxC2Vf %p\n\
  rx_cp_eAxC2Vf %p\n\
  rx_up_eAxC2Vf %p\n\
  log_level %d\n\
  max_sections_per_slot %d\n\
  max_sections_per_symbol %d\n",
      fh_config->bbdev_enc,
      fh_config->bbdev_dec,
      fh_config->tx_cp_eAxC2Vf,
      fh_config->tx_up_eAxC2Vf,
      fh_config->rx_cp_eAxC2Vf,
      fh_config->rx_up_eAxC2Vf,
      fh_config->log_level,
      fh_config->max_sections_per_slot,
      fh_config->max_sections_per_symbol);
}

static const paramdef_t *gpd(const paramdef_t *pd, int num, const char *name)
{
  /* the config module does not know const-correctness... */
  int idx = config_paramidx_fromname((paramdef_t *)pd, num, (char *)name);
  DevAssert(idx >= 0);
  return &pd[idx];
}

static uint64_t get_u64_mask(const paramdef_t *pd)
{
  DevAssert(pd != NULL);
  AssertFatal(pd->numelt > 0, "no entries for creation of mask\n");
  uint64_t mask = 0;
  for (int i = 0; i < pd->numelt; ++i) {
    int num = pd->iptr[i];
    AssertFatal(num >= 0 && num < 64, "cannot put element of %d in 64-bit mask\n", num);
    mask |= 1 << num;
  }
  return mask;
}

static bool set_fh_io_cfg(struct xran_io_cfg *io_cfg, const paramdef_t *fhip, int nump, const int num_rus)
{
  DevAssert(fhip != NULL);
  int num_dev = gpd(fhip, nump, ORAN_CONFIG_DPDK_DEVICES)->numelt;
  AssertFatal(num_dev > 0, "need to provide DPDK devices for O-RAN 7.2 Fronthaul\n");
  AssertFatal(num_dev < 17, "too many DPDK devices for O-RAN 7.2 Fronthaul\n");

  io_cfg->id = 0; // 0 -> xran as O-DU; 1 -> xran as O-RU
  io_cfg->num_vfs = num_dev; // number of VFs for C-plane and U-plane (should be even); max = XRAN_VF_MAX
  io_cfg->num_rxq = 1; // number of RX queues per VF
  for (int i = 0; i < num_dev; ++i) {
    io_cfg->dpdk_dev[i] = strdup(gpd(fhip, nump, ORAN_CONFIG_DPDK_DEVICES)->strlistptr[i]); // VFs devices
  }
  io_cfg->bbdev_dev[0] = NULL; // BBDev dev name; max devices = 1
  io_cfg->bbdev_mode = XRAN_BBDEV_NOT_USED; // DPDK for BBDev
  io_cfg->dpdkIoVaMode = 0; // IOVA mode
  io_cfg->dpdkMemorySize = *gpd(fhip, nump, ORAN_CONFIG_DPDK_MEM_SIZE)->uptr; // DPDK max memory allocation

  /* the following core assignment is needed for rte_eal_init() function within xran library;
    these parameters are machine specific */
  io_cfg->core = *gpd(fhip, nump, ORAN_CONFIG_IO_CORE)->iptr; // core used for IO; absolute CPU core ID for xran library, it should be an isolated core
  io_cfg->system_core = *gpd(fhip, nump, ORAN_CONFIG_SYSTEM_CORE)->iptr; // absolute CPU core ID for DPDK control threads, it should be an isolated core
  io_cfg->pkt_proc_core = get_u64_mask(gpd(fhip, nump, ORAN_CONFIG_WORKER_CORES)); // worker mask 0-63
  io_cfg->pkt_proc_core_64_127 = 0x0; // worker mask 64-127; to be used if machine supports more than 64 cores
  io_cfg->pkt_aux_core = 0; // sample app says 0 = "do not start"
  io_cfg->timing_core = *gpd(fhip, nump, ORAN_CONFIG_IO_CORE)->iptr; // core used by xran

  // io_cfg->port[XRAN_VF_MAX] // VFs ports; filled within xran library
  io_cfg->io_sleep = 0; // enable sleep on PMD cores; 0 -> no sleep
  io_cfg->nEthLinePerPort = *gpd(fhip, nump, ORAN_CONFIG_NETHPERPORT)->uptr; // 1, 2, 3 total number of links per O-RU (Fronthaul Ethernet link)
  io_cfg->nEthLineSpeed = *gpd(fhip, nump, ORAN_CONFIG_NETHSPEED)->uptr; // 10G,25G,40G,100G speed of Physical connection on O-RU
  io_cfg->one_vf_cu_plane = (io_cfg->num_vfs == num_rus); // C-plane and U-plane use one VF

  /* use owdm to calculate T12 and T34 -> CUS specification, section 2.3.3.3 */
  // io_cfg->eowd_cmn[2] // ecpri one-way delay measurements common settings for O-DU and O-RU
  // io_cfg->eowd_port[2][XRAN_VF_MAX] // ecpri owd measurements per port variables for O-DU and O-RU

  return true;
}

static bool set_fh_eaxcid_conf(struct xran_eaxcid_config *eaxcid_conf, enum xran_category cat)
{
  /* CUS specification, section 3.1.3.1.6
    DU_port_ID - used to differentiate processing units at O-DU (e.g., different baseband cards).
    BandSector_ID - aggregated cell identifier (distinguishes bands and sectors supported by the O-RU).
    CC_ID - distinguishes Carrier Components supported by the O-RU.
    RU_Port_ID - designates logical flows such as data layers or spatial streams, and logical flows such as separate
                 numerologies (e.g. PRACH) or signaling channels requiring special antenna assignments such as SRS.
    The assignment of the DU_port_ID, BandSector_ID, CC_ID, and RU_Port_ID
    as part of the eAxC ID is done solely by the O-DU via the M-plane.
    Each ID field has a flexible bit allocation, but the total eAxC ID field length is fixed, 16 bits. */
  switch (cat) {
    case XRAN_CATEGORY_A:
      eaxcid_conf->mask_cuPortId = 0xf000;
      eaxcid_conf->mask_bandSectorId = 0x0f00;
      eaxcid_conf->mask_ccId = 0x00f0;
      eaxcid_conf->mask_ruPortId = 0x000f;
      eaxcid_conf->bit_cuPortId = 0;
      eaxcid_conf->bit_bandSectorId = 0; // total number of band sectors supported by O-RU should be retreived by M-plane - <max-num-bands> && <max-num-sectors>
      eaxcid_conf->bit_ccId = 0; // total number of CC supported by O-RU should be retreived by M-plane - <max-num-component-carriers>
      eaxcid_conf->bit_ruPortId = 0;
      break;
    case XRAN_CATEGORY_B:
      eaxcid_conf->mask_cuPortId = 0xf000;
      eaxcid_conf->mask_bandSectorId = 0x0c00;
      eaxcid_conf->mask_ccId = 0x0300;
      eaxcid_conf->mask_ruPortId = 0x000f;
      eaxcid_conf->bit_cuPortId = 12;
      eaxcid_conf->bit_bandSectorId = 10;
      eaxcid_conf->bit_ccId = 8;
      eaxcid_conf->bit_ruPortId = 0;
      break;
    default:
      return false;
  }

  return true;
}

uint8_t *get_ether_addr(const char *addr, struct rte_ether_addr *ether_addr)
{
#pragma GCC diagnostic push
  // the following line disables the deprecated warning
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
  int ret = rte_ether_unformat_addr(addr, ether_addr);
#pragma GCC diagnostic pop
  if (ret == 0)
    return (uint8_t *)ether_addr;
  return NULL;
}

static bool set_fh_init(struct xran_fh_init *fh_init, enum xran_category xran_cat)
{
  memset(fh_init, 0, sizeof(*fh_init));

  // verify oran section is present: we don't have a list but the below returns
  // numelt > 0 if the block is there
  paramlist_def_t pl = {0};
  strncpy(pl.listname, CONFIG_STRING_ORAN, sizeof(pl.listname) - 1);
  config_getlist(config_get_if(), &pl, NULL, 0, /* prefix */ NULL);
  if (pl.numelt == 0) {
    printf("Configuration section \"%s\" not present: cannot initialize fhi_lib!\n", CONFIG_STRING_ORAN);
    return false;
  }

  paramdef_t fhip[] = ORAN_GLOBALPARAMS_DESC;
  int nump = sizeofArray(fhip);
  int ret = config_get(config_get_if(), fhip, nump, CONFIG_STRING_ORAN);
  if (ret <= 0) {
    printf("problem reading section \"%s\"\n", CONFIG_STRING_ORAN);
    return false;
  }

  paramdef_t FHconfigs[] = ORAN_FH_DESC;
  paramlist_def_t FH_ConfigList = {CONFIG_STRING_ORAN_FH};
  char aprefix[MAX_OPTNAME_SIZE] = {0};
  sprintf(aprefix, "%s", CONFIG_STRING_ORAN);
  const int nfh = sizeofArray(FHconfigs);
  config_getlist(config_get_if(), &FH_ConfigList, FHconfigs, nfh, aprefix);

  int num_rus = FH_ConfigList.numelt; // based on the number of fh_config sections -> number of RUs

  if (!set_fh_io_cfg(&fh_init->io_cfg, fhip, nump, num_rus))
    return false;
  if (!set_fh_eaxcid_conf(&fh_init->eAxCId_conf, xran_cat))
    return false;

  fh_init->xran_ports = num_rus; // since we use xran as O-DU, xran_ports is set to the number of RUs
  fh_init->dpdkBasebandFecMode = 0; // DPDK Baseband FEC device mode (0-SW, 1-HW); not used in xran
  fh_init->dpdkBasebandDevice = NULL; // DPDK Baseband device address; not used in xran
  /* used to specify a unique prefix for shared memory, and files created by multiple DPDK processes;
    is it necessary */
  fh_init->filePrefix = strdup(*gpd(fhip, nump, ORAN_CONFIG_FILE_PREFIX)->strptr);
  /* maximum transmission unit (MTU) is the size of the largest protocol data unit (PDU) that can be
    communicated in a single xRAN network layer transaction. Supported 1500 bytes and 9600 bytes (Jumbo Frame);
    xran only checks if (MTU <= 1500), therefore setting any value > 1500, xran assumes 9600 value is used */
  fh_init->mtu = *gpd(fhip, nump, ORAN_CONFIG_MTU)->uptr;

  fh_init->p_o_du_addr = NULL; // DPDK retreives DU MAC address within the xran library with rte_eth_macaddr_get() function

  int num_ru_addr = gpd(fhip, nump, ORAN_CONFIG_RU_ADDR)->numelt;
  fh_init->p_o_ru_addr = calloc(num_ru_addr, sizeof(struct rte_ether_addr));
  char **ru_addrs = gpd(fhip, nump, ORAN_CONFIG_RU_ADDR)->strlistptr;
  AssertFatal(fh_init->p_o_ru_addr != NULL, "out of memory\n");
  for (int i = 0; i < num_ru_addr; ++i) {
    struct rte_ether_addr *ea = (struct rte_ether_addr *)fh_init->p_o_ru_addr;
    if (get_ether_addr(ru_addrs[i], &ea[i]) == NULL) {
      printf("could not read ethernet address '%s' for RU!\n", ru_addrs[i]);
      return false;
    }
  }

  fh_init->totalBfWeights = 0; // only used if id = O_RU (for emulation); C-plane extension types; section 5.4.6 of CUS spec

  return true;
}

// PRACH guard interval. Raymond: "[it] is not in the configuration, (i.e. it
// is deterministic depending on others). LiteON must hard-code this in the
// O-RU itself, benetel doesn't (as O-RAN specifies). So we will need to tell
// the driver what the case is and provide"
// this is a hack
int g_kbar;

static bool set_fh_prach_config(const openair0_config_t *oai0,
                                const uint32_t max_num_ant,
                                const paramdef_t *prachp,
                                int nprach,
                                struct xran_prach_config *prach_config)
{
  const split7_config_t *s7cfg = &oai0->split7;

  prach_config->nPrachConfIdx = s7cfg->prach_index; // PRACH Configuration Index
  prach_config->nPrachSubcSpacing = oai0->nr_scs_for_raster; // 0 -> 15kHz, 1 -> 30kHz, 2 -> 60kHz, 3 -> 120kHz
  prach_config->nPrachZeroCorrConf = 0; // PRACH zeroCorrelationZoneConfig; should be saved from config file; not used in xran
  prach_config->nPrachRestrictSet = 0; /* PRACH restrictedSetConfig; should be saved from config file; 0 = unrestricted,
                                          1 = restricted type A, 2=restricted type B; not used in xran */
  prach_config->nPrachRootSeqIdx = 0; // PRACH Root Sequence Index; should be saved from config file; 1 = 839, 2 = 139; not used in xran
  prach_config->nPrachFreqStart = s7cfg->prach_freq_start; // PRACH frequency start (MSG1)
  prach_config->nPrachFreqOffset = (s7cfg->prach_freq_start * 12 - oai0->num_rb_dl * 6) * 2; // PRACH frequency offset
  prach_config->nPrachFilterIdx = 0; /* PRACH filter index; not used in xran;
                                        in E release hardcoded to XRAN_FILTERINDEX_PRACH_ABC (preamble format A1~3, B1~4, C0, C2)
                                        in F release properly calculated */

  /* Return values after initialization */
  prach_config->startSymId = 0;
  prach_config->lastSymId = 0;
  prach_config->startPrbc = 0;
  prach_config->numPrbc = 0;
  prach_config->timeOffset = 0;
  prach_config->freqOffset = 0;

  /* xran defines PDSCH eAxC IDs as [0...Ntx-1];
     xran defines PUSCH eAxC IDs as [0...Nrx-1];
     PRACH offset must be >= max(Ntx, Nrx) */
  uint8_t offset = *gpd(prachp, nprach, ORAN_PRACH_CONFIG_EAXC_OFFSET)->u8ptr;
  prach_config->eAxC_offset = (offset != 0) ? offset : max_num_ant;

  g_kbar = *gpd(prachp, nprach, ORAN_PRACH_CONFIG_KBAR)->uptr;

  return true;
}

static bool set_fh_frame_config(const openair0_config_t *oai0, struct xran_frame_config *frame_config)
{
  const split7_config_t *s7cfg = &oai0->split7;
  frame_config->nFrameDuplexType = oai0->duplex_mode == duplex_mode_TDD ? XRAN_TDD : XRAN_FDD; // Frame Duplex type:  0 -> FDD, 1 -> TDD
  frame_config->nNumerology = oai0->nr_scs_for_raster; /* 0 -> 15kHz,  1 -> 30kHz,  2 -> 60kHz
                                                          3 -> 120kHz, 4 -> 240kHz */

  if (frame_config->nFrameDuplexType == XRAN_FDD)
    return true;

  // TDD periodicity
  frame_config->nTddPeriod = s7cfg->n_tdd_period;

  // TDD Slot configuration
  struct xran_slot_config *sc = &frame_config->sSlotConfig[0];
  for (int slot = 0; slot < frame_config->nTddPeriod; ++slot)
    for (int sym = 0; sym < 14; ++sym)
      sc[slot].nSymbolType[sym] = s7cfg->slot_dirs[slot].sym_dir[sym];

  return true;
}

static bool set_fh_ru_config(const paramdef_t *rup, uint16_t fftSize, int nru, enum xran_category xran_cat, struct xran_ru_config *ru_config)
{
  ru_config->xranTech = XRAN_RAN_5GNR; // 5GNR or LTE
  ru_config->xranCat = xran_cat; // mode: Catergory A or Category B
  ru_config->xranCompHdrType = XRAN_COMP_HDR_TYPE_STATIC; // dynamic or static udCompHdr handling
  ru_config->iqWidth = *gpd(rup, nru, ORAN_RU_CONFIG_IQWIDTH)->uptr; // IQ bit width
  AssertFatal(ru_config->iqWidth <= 16, "IQ Width cannot be > 16!\n");
  ru_config->compMeth = ru_config->iqWidth < 16 ? XRAN_COMPMETHOD_BLKFLOAT : XRAN_COMPMETHOD_NONE; // compression method
  ru_config->iqWidth_PRACH = *gpd(rup, nru, ORAN_RU_CONFIG_IQWIDTH_PRACH)->uptr; // IQ bit width for PRACH
  AssertFatal(ru_config->iqWidth_PRACH <= 16, "IQ Width for PRACH cannot be > 16!\n");
  ru_config->compMeth_PRACH = ru_config->iqWidth_PRACH < 16 ? XRAN_COMPMETHOD_BLKFLOAT : XRAN_COMPMETHOD_NONE; // compression method for PRACH
  AssertFatal(fftSize > 0, "FFT size cannot be 0\n");
  ru_config->fftSize = fftSize; // FFT Size
  ru_config->byteOrder = XRAN_NE_BE_BYTE_ORDER; // order of bytes in int16_t in buffer; big or little endian
  ru_config->iqOrder = XRAN_I_Q_ORDER; // order of IQs in the buffer
  ru_config->xran_max_frame = 0; // max frame number supported; if not specified, default of 1023 is used
  return true;
}

static bool set_maxmin_pd(const paramdef_t *pd, int num, const char *name, uint16_t *min, uint16_t *max)
{
  const paramdef_t *p = gpd(pd, num, name);
  if (p->numelt != 2) {
    printf("parameter list \"%s\" should have exactly two parameters (max&min), but has %d\n", name, num);
    return false;
  }
  *min = p->uptr[0];
  *max = p->uptr[1];
  if (*min > *max) {
    printf("min parameter of \"%s\" is larger than max!\n", name);
    return false;
  }
  return true;
}

static bool set_fh_config(int ru_idx, int num_rus, enum xran_category xran_cat, const openair0_config_t *oai0, struct xran_fh_config *fh_config)
{
  AssertFatal(num_rus == 1 || num_rus == 2, "only support 1 or 2 RUs as of now\n");
  AssertFatal(ru_idx < num_rus, "illegal ru_idx %d: must be < %d\n", ru_idx, num_rus);
  DevAssert(oai0->tx_num_channels > 0 && oai0->rx_num_channels > 0);
  DevAssert(oai0->tx_bw > 0 && oai0->rx_bw > 0);
  DevAssert(oai0->tx_freq[0] > 0);
  for (int i = 1; i < oai0->tx_num_channels; ++i)
    DevAssert(oai0->tx_freq[0] == oai0->tx_freq[i]);
  DevAssert(oai0->rx_freq[0] > 0);
  for (int i = 1; i < oai0->rx_num_channels; ++i)
    DevAssert(oai0->rx_freq[0] == oai0->rx_freq[i]);
  DevAssert(oai0->nr_band > 0);
  DevAssert(oai0->nr_scs_for_raster > 0);
  AssertFatal(oai0->threequarter_fs == 0, "cannot use three-quarter sampling with O-RAN 7.2 split\n");

  paramdef_t FHconfigs[] = ORAN_FH_DESC;
  paramlist_def_t FH_ConfigList = {CONFIG_STRING_ORAN_FH};
  char aprefix[MAX_OPTNAME_SIZE] = {0};
  sprintf(aprefix, "%s", CONFIG_STRING_ORAN);
  const int nfh = sizeofArray(FHconfigs);
  config_getlist(config_get_if(), &FH_ConfigList, FHconfigs, nfh, aprefix);
  if (FH_ConfigList.numelt == 0) {
    printf("No configuration section \"%s\" found inside \"%s\": cannot initialize fhi_lib!\n", CONFIG_STRING_ORAN_FH, aprefix);
    return false;
  }
  paramdef_t *fhp = FH_ConfigList.paramarray[ru_idx];

  paramdef_t rup[] = ORAN_RU_DESC;
  int nru = sizeofArray(rup);
  sprintf(aprefix, "%s.%s.[%d].%s", CONFIG_STRING_ORAN, CONFIG_STRING_ORAN_FH, ru_idx, CONFIG_STRING_ORAN_RU);
  int ret = config_get(config_get_if(), rup, nru, aprefix);
  if (ret < 0) {
    printf("No configuration section \"%s\": cannot initialize fhi_lib!\n", aprefix);
    return false;
  }
  paramdef_t prachp[] = ORAN_PRACH_DESC;
  int nprach = sizeofArray(prachp);
  sprintf(aprefix, "%s.%s.[%d].%s", CONFIG_STRING_ORAN, CONFIG_STRING_ORAN_FH, ru_idx, CONFIG_STRING_ORAN_PRACH);
  ret = config_get(config_get_if(), prachp, nprach, aprefix);
  if (ret < 0) {
    printf("No configuration section \"%s\": cannot initialize fhi_lib!\n", aprefix);
    return false;
  }

  memset(fh_config, 0, sizeof(*fh_config));

  fh_config->dpdk_port = ru_idx; // DPDK port number used for FH
  fh_config->sector_id = 0; // Band sector ID for FH; not used in xran
  fh_config->nCC = 1; // number of Component carriers supported on FH; M-plane info
  fh_config->neAxc = RTE_MAX(oai0->tx_num_channels / num_rus, oai0->rx_num_channels / num_rus); // number of eAxc supported on one CC = max(PDSCH, PUSCH)
  fh_config->neAxcUl = 0; // number of eAxc supported on one CC for UL direction = PUSCH; used only if XRAN_CATEGORY_B
  fh_config->nAntElmTRx = 0; // number of antenna elements for TX and RX = SRS; used only if XRAN_CATEGORY_B
  fh_config->nDLFftSize = 0; // DL FFT size; not used in xran
  fh_config->nULFftSize = 0; // UL FFT size; not used in xran
  fh_config->nDLRBs = oai0->num_rb_dl; // DL PRB
  fh_config->nULRBs = oai0->num_rb_dl; // UL PRB; in xran not used as id = O_DU,  but used in oaioran.c/oran-init.c
  fh_config->nDLAbsFrePointA = 0; // Abs Freq Point A of the Carrier Center Frequency for in KHz Value; not used in xran
  fh_config->nULAbsFrePointA = 0; // Abs Freq Point A of the Carrier Center Frequency for in KHz Value; not used in xran
  fh_config->nDLCenterFreqARFCN = 0; // center frequency for DL in NR-ARFCN; not used in xran
  fh_config->nULCenterFreqARFCN = 0; // center frequency for UL in NR-ARFCN; not used in xran
  fh_config->ttiCb = NULL; // check tti_to_phy_cb(), tx_cp_dl_cb() and tx_cp_ul_cb => first_call
  fh_config->ttiCbParam = NULL; // check tti_to_phy_cb(), tx_cp_dl_cb() and tx_cp_ul_cb => first_call
  fh_config->Tadv_cp_dl = *gpd(fhp, nfh, ORAN_FH_CONFIG_TADV_CP_DL)->uptr; // not used in xran
  if (!set_maxmin_pd(fhp, nfh, ORAN_FH_CONFIG_T2A_CP_DL, &fh_config->T2a_min_cp_dl, &fh_config->T2a_max_cp_dl)) // not used in xran
    return false;
  if (!set_maxmin_pd(fhp, nfh, ORAN_FH_CONFIG_T2A_CP_UL, &fh_config->T2a_min_cp_ul, &fh_config->T2a_max_cp_ul)) // not used in xran
    return false;
  if (!set_maxmin_pd(fhp, nfh, ORAN_FH_CONFIG_T2A_UP, &fh_config->T2a_min_up, &fh_config->T2a_max_up)) // not used in xran
    return false;
  if (!set_maxmin_pd(fhp, nfh, ORAN_FH_CONFIG_TA3, &fh_config->Ta3_min, &fh_config->Ta3_max)) // not used in xran
    return false;
  if (!set_maxmin_pd(fhp, nfh, ORAN_FH_CONFIG_T1A_CP_DL, &fh_config->T1a_min_cp_dl, &fh_config->T1a_max_cp_dl))
    return false;
  if (!set_maxmin_pd(fhp, nfh, ORAN_FH_CONFIG_T1A_CP_UL, &fh_config->T1a_min_cp_ul, &fh_config->T1a_max_cp_ul))
    return false;
  if (!set_maxmin_pd(fhp, nfh, ORAN_FH_CONFIG_T1A_UP, &fh_config->T1a_min_up, &fh_config->T1a_max_up))
    return false;
  if (!set_maxmin_pd(fhp, nfh, ORAN_FH_CONFIG_TA4, &fh_config->Ta4_min, &fh_config->Ta4_max))
    return false;
  fh_config->enableCP = 1; // enable C-plane
  fh_config->prachEnable = 1; // enable PRACH
  fh_config->srsEnable = 0; // enable SRS; used only if XRAN_CATEGORY_B
  fh_config->puschMaskEnable = 0; // enable PUSCH mask; only used if id = O_RU
  fh_config->puschMaskSlot = 0; // specific which slot PUSCH channel masked; only used if id = O_RU
  fh_config->cp_vlan_tag = *gpd(fhp, nfh, ORAN_FH_CONFIG_CP_VLAN_TAG)->uptr; // C-plane VLAN tag; not used in xran; needed for M-plane
  fh_config->up_vlan_tag = *gpd(fhp, nfh, ORAN_FH_CONFIG_UP_VLAN_TAG)->uptr; // U-plane VLAN tag; not used in xran; needed for M-plane
  fh_config->debugStop = 0; // enable auto stop; not used in xran
  fh_config->debugStopCount = 0; // enable auto stop after number of Tx packets; not used in xran
  fh_config->DynamicSectionEna = 0; // enable dynamic C-Plane section allocation; not used in xran
  fh_config->GPS_Alpha = 0; // refers to alpha as defined in section 9.7.2 of ORAN spec. this value should be alpha*(1/1.2288ns), range 0 - 1e7 (ns); offset_nsec = (pConf->GPS_Beta - offset_sec * 100) * 1e7 + pConf->GPS_Alpha
  fh_config->GPS_Beta = 0; // beta value as defined in section 9.7.2 of ORAN spec. range -32767 ~ +32767; offset_sec = pConf->GPS_Beta / 100

  if (!set_fh_prach_config(oai0, fh_config->neAxc, prachp, nprach, &fh_config->prach_conf))
    return false;
  /* SRS only used if XRAN_CATEGORY_B
    Note: srs_config->eAxC_offset >= prach_config->eAxC_offset + PRACH */
  // fh_config->srs_conf = {0};
  if (!set_fh_frame_config(oai0, &fh_config->frame_conf))
    return false;
  if (!set_fh_ru_config(rup, oai0->split7.fftSize, nru, xran_cat, &fh_config->ru_conf))
    return false;

  fh_config->bbdev_enc = NULL; // call back to poll BBDev encoder
  fh_config->bbdev_dec = NULL; // call back to poll BBDev decoder

  /* CUS specification, section 3.1.3.1.6 
    This parameter is an eAxC identifier (eAxC ID) and identifies the specific data flow associated with each
    C-Plane (ecpriRtcid) or U-Plane (ecpriPcid) message.
    Each of bellow parameters is a matrix [XRAN_MAX_SECTOR_NR][XRAN_MAX_ANTENNA_NR*2 + XRAN_MAX_ANT_ARRAY_ELM_NR] */
  // fh_config->tx_cp_eAxC2Vf // mapping of C-Plane (ecpriRtcid) to VF; not used in xran
  // fh_config->tx_up_eAxC2Vf // mapping of U-Plane (ecpriPcid) to VF; not used in xran
  // fh_config->rx_cp_eAxC2Vf // mapping of C-Plane (ecpriRtcid) to VF; not used in xran
  // fh_config->rx_up_eAxC2Vf // mapping of U-Plane (ecpriPcid) to VF; not used in xran

  fh_config->log_level = 1; // configuration of log level; 1 -> enabled

  /* Parameters that should be retreived via M-plane
    O-RU defines them for Section Type 1 (Most DL/UL radio channels) and 3 (PRACH and mixed-numerology channels)
    Note: When PRACH having same numerology as other UL channels, Section type 1 can alternatively be used by O-DU for PRACH signaling.
          In this case, O-RU is not expected to perform any PRACH specific processing. */
  fh_config->max_sections_per_slot = 0; // not used in xran
  fh_config->max_sections_per_symbol = 0; // not used in xran

  return true;
}

bool get_xran_config(const openair0_config_t *openair0_cfg, struct xran_fh_init *fh_init, struct xran_fh_config *fh_config)
{
  /* This xran integration release is only valid for O-RU CAT A.
    Therefore, each FH parameter is hardcoded to CAT A.
    If you are interested in CAT B, please be aware that parameters of fh_init and fh_config structs must be modified accordingly. */
  enum xran_category xran_cat = XRAN_CATEGORY_A;

  if (!set_fh_init(fh_init, xran_cat)) {
    printf("could not read FHI 7.2/ORAN config\n");
    return false;
  }

  for (int32_t o_xu_id = 0; o_xu_id < fh_init->xran_ports; o_xu_id++) {
    if (!set_fh_config(o_xu_id, fh_init->xran_ports, xran_cat, openair0_cfg, &fh_config[o_xu_id])) {
      printf("could not read FHI 7.2/RU-specific config\n");
      return false;
    }
  }

  return true;
}
