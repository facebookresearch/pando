#pragma once

#include <array>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "session_manager.h"

namespace pnd {
namespace pando {

/** The main libpando interface.
 * Configures devices, starts and stops experiments, publishes data over ZMQ, and logs data to HDF5
 * files.
 */
struct PandoInterface : public common::SessionManager<> {
  /** ZMQ endpoint of the main PUB socket (in-process transport). */
  static constexpr char kProxyEgressInprocEp[] = "inproc://pando_proxy_egress";
  /** ZMQ endpoint of the ingress port of the internal ZMQ proxy. */
  static constexpr char kProxyIngressInprocEp[] = "inproc://pando_proxy_ingress";
  /** ZMQ endpoint of the main PUB socket (TCP transport). */
  static constexpr int kProxyEgressTcpStartPort = 5060;
  /** ZMQ endpoint of the main PUB socket (TCP transport). */
  static constexpr int kProxyEgressTcpEndPort = 5064;

  /** Supported photon data acquisition devices. */
  enum DeviceType {
    NONE, /**< Don't collect data from a device. */
    HYDRA_HARP_T2, /**< The PicoQuant HydraHarp in T2 mode. */
    HYDRA_HARP_T3, /**< The PicoQuant HydraHarp in T3 mode. */
    PICO_HARP_T2, /**< The PicoQuant PicoHarp in T2 mode. */
    PICO_HARP_T3, /**< The PicoQuant PicoHarp in T3 mode. */
    MULTI_HARP_T2, /**< The PicoQuant MultiHarp in T2 mode. */
    MULTI_HARP_T3, /**< The PicoQuant MultiHarp in T3 mode. */
    PF32_DCS, /**< The Photon Force PF32 camera in DCS mode. */
    PF32_TCSPC, /**< The Photon Force PF32 camera in TCSPC mode. */
    PF32_G2, /**< The Photon Force PF32 camera in correlator mode. */
    /** Replay a PTU file generated by the PicoQuant software using the HydraHarp in T2 mode. */
    HYDRA_HARP_T2_MOCK_PTU,
    /** Replay a PTU file generated by the PicoQuant software using the HydraHarp in T3 mode. */
    HYDRA_HARP_T3_MOCK_PTU,
    /** Replay a PTU file generated by the PicoQuant software using the PicoHarp in T2 mode. */
    PICO_HARP_T2_MOCK_PTU,
    /** Replay a PTU file generated by the PicoQuant software using the PicoHarp in T3 mode. */
    PICO_HARP_T3_MOCK_PTU,
    /** Replay a PTU file generated by the PicoQuant software using the MultiHarp in T2 mode. */
    MULTI_HARP_T2_MOCK_PTU,
    /** Replay a PTU file generated by the PicoQuant software using the MultiHarp in T3 mode. */
    MULTI_HARP_T3_MOCK_PTU,
    /** Replay a raw HDF5 file (produced by Pando) containing data produced by a timetagger. */
    TIMETAGGER_MOCK_HDF5,
    /** Replay a raw HDF5 file (produced by Pando) containing data produced by a histogrammer. */
    HISTOGRAMMER_MOCK_HDF5,
    /** The Basler a2A1920-160umPRO USB 3.0 CMOS camera. */
    BASLER_A2,
    /** Replay a raw HDF5 file (produced by Pando) containing data produced by a camera. */
    CAMERA_MOCK_HDF5,
    /** The Hamamatsu ORCA-fusion BT CoaXPress CMOS Camera. */
    ORCA_FUSION,
    /** A Basler Ace Classic camera paired with microEnable 5 marathon Camera Link framegrabber. */
    BASLER_ACA2000_CL,
    /** A Basler Boost Camera paired with a CXP12 interface card. */
    BASLER_BOOST,
  };

  /** Specifies which calculations to perform on photon data. */
  enum class ExperimentType {
    NONE, /**< Don't process raw data. */
    TD, /**< Calculate DTOF and CRi. */
    DCS, /**< Calculate g2. */
    TDDCS, /**< Calculate DTOF, CRi, and g2 (or g2i, if Config::calc_g2i is true). */
  };

  /** The PF32 supports (via dedicated bitfiles) multiple word sizes in DCS and TCSPC modes. */
  enum class PF32BitMode {
    SIXTEEN,
    EIGHT,
    FOUR,
  };

  /** A collection of all available experiment configuration settings.
   * Many settings only apply to a particular device or experiment type.
   */
  struct Config {
    /** The device to acquire photon data from. */
    DeviceType device = DeviceType::NONE;
    /** The calculations to perform on the acquired photon data. Each experiment type is only
    available on devices that it is applicable to. */
    ExperimentType exp_type = ExperimentType::NONE;
    /** The HDF5 or PTU file to replay data from. Applies to devices #HYDRA_HARP_T2_MOCK_PTU,
     * #HYDRA_HARP_T3_MOCK_PTU, #PICO_HARP_T2_MOCK_PTU, #PICO_HARP_T3_MOCK_PTU,
     * #MULTI_HARP_T2_MOCK_PTU, #MULTI_HARP_T3_MOCK_PTU, #TIMETAGGER_MOCK_HDF5, and
     * #HISTOGRAMMER_MOCK_HDF5 */
    std::string mock_file = "";
    std::string output_dir = "."; /**< The directory to which HDF5 files will be saved. */
    /** The starting (before any exponential rebinning) bin size histograms fed into the g2
     * algorithm. Put another way: The starting tau step size of g2 curves. */
    int bin_size_ns = 1000;
    /** Number of tau values to consider in the g2 calculation before performing rebinning and
    proceeding to the next level. Points that*/
    int points_per_level = 8;
    /* Number of levels to compute in the exponential rebinning algorithm. Rebinning occurs
     * (#n_levels - 1) times. */
    int n_levels = 12;
    /** The number of adjacent bins that are coalesced into each new bin during rebinning. */
    int rebin_factor = 2;
    /** The g2 integration period (T_int). Expressed in terms of the bin width at the
    final level of the exponential rebinning algorithm (that is, \f$T_{int} = final\_bin\_count
    \cdot bin\_size\_ns \cdot rebin\_factor ^ {n\_levels - 1}\f$). */
    int final_bin_count = 32;
    /** Calculate g2 in per-channel ROIs defined by #cri_offset and #cri_width. */
    bool calc_g2i = false;
    /** The list of channel indices to acquire and process data from. Depending on the device,
     * disabled channels may still be "acquired from", but the resulting data is discarded as soon
     * as is practical. */
    std::list<int32_t> enabled_channels = {0};
    bool pf32_laser_master = true;
    PF32BitMode pf32_bit_mode = PF32BitMode::SIXTEEN;
    /** [Hydra/Pico]Harp CFD discriminator level. Map from channel index to value in mV. */
    std::map<int, int> xharp_level;
    /** [Hydra/Pico]Harp zero cross level. Map from channel index to value in mV. */
    std::map<int, int> xharp_zerox;
    /** [Hydra/Pico/Multi]Harp sync divider level. Passed directly to the SetSyncDiv method of the
     * PicoQuant APIs (has no direct effect on Pando.) */
    int xharp_sync_div = 8;
    int xharp_sync_level = 30;
    int xharp_sync_zerox = 10;
    int xharp_sync_offset = -10000;
    int multi_harp_sync_trigger_edge = 0; /**< 0 - falling, 1 - rising */
    int multi_harp_sync_trigger_level_mv = 0; /**< trigger level for sync input (mV) */
    int multi_harp_sync_tdead_ps = 0; /**< programmable dead time (picoseconds) */
    /** The period of the laser sync signal (in picoseconds) for pulsed laser experiments.
    For PicoQuant devices in T3 mode, this is directly used to convert laser sync counts into
    macrotimes. For the PF32 in TCSPC mode, it's used to convert reverse start-stop measurements
    into microtimes.*/
    uint64_t laser_sync_period_ps = 20000; /**< [Hydra/Pico]Harp laser sync period in ps */
    /** [Hydra/Multi]Harp input channel offsets. Map from channel index to value in
     * picoseconds. Note: despite the name, applies to MultiHarp as well. */
    std::map<int, int> hharp_input_offsets;
    std::map<int, int> multi_harp_trigger_edge;
    std::map<int, int> multi_harp_trigger_level_mv;
    std::map<int, int> multi_harp_tdead_ps;
    /** Path to FPGA bitfile */
    std::string pf32_firmware_path =
        "./third-party/pf32_api/pf32_api/bitfiles/PF32_USB3_[1864-8bit].bit";
    int dtof_range_min = 0; /**< Bottom of range for which DTOF will be calculated */
    int dtof_range_max = 20000; /**< Top of range for which DTOF will be calculated */
    /** Left edges of microtime ROIs (map from channel index to picoseconds). */
    std::map<int, int> cri_offset;
    /** Widths of microtime ROIs (map from channel index to picoseconds). */
    std::map<int, int> cri_width;
    bool publish_raw_data = false; /**< If true, raw data is protobuffed and published over ZMQ */
    /** If true, `sawyer_raw_[timestamp].h5` is created and corresponding data is logged. */
    bool log_raw_data = true;
    /** If true, `sawyer_analyzed_[timestamp].h5` is created and corresponding data is logged. */
    bool log_analyzed_data = true;
    /** If true, `sawyer_peripheral_[timestamp].h5` is created and corresponding data is logged. */
    bool log_peripheral_data = true;
    /** Default value of integration period. Update if other defaults change. */
    uint64_t count_integ_period_ns = 65536000;
    uint64_t dtof_integ_period_ns = 65536000;
    uint64_t cri_integ_period_ns = 65536000;
    /** Attempt to connect to an instance of Pandoboxd running at #pandoboxd_ip_addr. */
    bool use_pandoboxd = false;
    /** An IP address where an instance of Pandoboxd can be reached. Typically that of the Sync
     * Box, which has a hard wired link-local address.*/
    std::string pandoboxd_ip_addr = "169.254.12.34";
    /** If true, shift timestamps into an externally defined time domain using a PPS signal present
    on marker channel 0. This applies to the timestamps of the datums we generate, but not the
    timestamps comprising the raw data. */
    bool use_pps = false;
    /** The period of the camera frame trigger to be generated by the Sync Box. 1 LSB = 10ns. */
    uint32_t camera_frame_trigger_period_10ns = 641026;
    /** The camera exposure time in us. */
    double camera_exposure_time_us = 6000;
    /** Configure the camera to transmit a test pattern instead of real data. */
    bool camera_enable_test_pattern = false;
    /** Apply a rectangular ROI to the camera at the lowest possible leve. May be used to increase
     * maximum FPS. */
    bool camera_enable_roi = false;
    uint32_t camera_roi_x_offset = 0;
    uint32_t camera_roi_y_offset = 0;
    uint32_t camera_roi_width = 0;
    uint32_t camera_roi_height = 0;
    /** The period of the auxillary frame trigger to be generated by the Sync Box. 1 LSB = 10ns. */
    uint32_t aux_frame_trigger_period_10ns = 0;
    /** The g2 integration period (T_int) when using device PF32_G2.
     * Expressed in terms of the bin width (synonomous with frame rate in this case) at the first
     * level of the exponential rebinning algorithm (that is, \f$T_{int} = pf32\_g2\_frame\_count
     * \cdot bin\_size\_ns  */
    int pf32_g2_frame_count = 40000;
    /** When true, t_ints are triggered by pulses on the SYNC input, and are non-contiguous. */
    bool pf32_g2_burst_mode = false;
    /** The number of adjacent bins that are coalesced into each new bin at each successive
     * stage of rebinning when using device PF32_G2. */
    int pf32_g2_rebin_factor_0 = 1;
    int pf32_g2_rebin_factor_1 = 1;
    int pf32_g2_rebin_factor_2 = 1;
    int pf32_g2_rebin_factor_3 = 1;
  };

  /** Apply a new set of configuration values.
   * This has four possible effects:
   *   - Any subsequently started experiment will use the settings in @p config.
   *   - Any in-progress experiment will immediately begin using a subset of settings in @p config
   *     that are runtime-editable.
   *   - If the new value of Config::device is different than the previous one, an attempt is made
   *     to connect to the new device.
   *   - If the new value of Config::use_pandoboxd is different than the previous one:
   *     1. Terminate any existing connection to a Pandoboxd instance
   *     2. If Config::use_pandoboxd is true, attempt to connect to a remote Pandoboxd instance.
   *
   * @param config The new configuration values.
   */
  virtual void Configure(const Config& config) = 0;

  /** Retrieve a copy of the current configuration. */
  virtual Config GetConfig() = 0;

  /** Returns false if any internal threads have crashed after throwing an exception. */
  virtual bool IsHealthy() = 0;

  /** Returns the zmq tcp proxy egress port. */
  virtual int GetProxyEgressPort() = 0;
};

/** Helper class for managing a global Pando instance. */
class SingletonPando {
 public:
  /** Get a shared_ptr to the global Pando instance, creating one if it doesn't exist. */
  static std::shared_ptr<PandoInterface> Get();

 private:
  static std::mutex creation_mutex_;
  static std::weak_ptr<PandoInterface> pando_wp_;
};

/** Emit a log message containing the information in version.h */
void LogVersion();

} // namespace pando
} // namespace pnd
