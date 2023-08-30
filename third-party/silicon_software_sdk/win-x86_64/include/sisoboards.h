/**
 * sisoboards.h
 *
 * Copyright (c) 2009-2017 Silicon Software GmbH, All Rights Reserved.
 *
 *
 * \file sisoboards.h
 *
 * \brief Board type constant definitions.
 *
 * \author Silicon Software GmbH
 */

#ifndef _SISOBOARDS_H
#define _SISOBOARDS_H

/**
 * Board type identifiers.
 *
 * Every device is identified by one of these identifiers. They
 * can be queried e.g. by Fg_getBoardType() or with
 * Fg_getParameterWithType() using the #FG_BOARD_INFORMATION argument.
 *
 * Please note that subvariants of the boards that are only different by the
 * physical layout of the connectors (e.g. CL connectors vs. PoCL connectors)
 * use the same type value.
 */
enum siso_board_type {
    PN_MICROENABLE = 0xa1,                             /**< microEnable I */
    PN_MICROENABLEII = 0xa2,                           /**< microEnable II */
    PN_MICROENABLE3I = 0xa3,                           /**< microEnable III */
    PN_MICROENABLE3IXXL = 0xa31,                       /**< microEnable III-XXL */

    PN_MICROENABLE4AD1CL = 0xa40,                      /**< microEnable IV AD1-CL */
    PN_MICROENABLE4BASE =
        PN_MICROENABLE4AD1CL,                          /**< \deprecated old name for PN_MICROENABLE4AD1CL, maintained only for source compatibility */
    PN_MICROENABLE4BASEx4 = 0xa43,                     /**< \deprecated name for a prototype never used*/
    PN_MICROENABLE4AD4CL = 0xa42,                      /**< microEnable IV AD4-CL */
    PN_MICROENABLE4VD1CL = 0xa41,                      /**< microEnable IV VD1-CL */
    PN_MICROENABLE4FULLx1 =
        PN_MICROENABLE4VD1CL,                          /**< \deprecated old name for PN_MICROENABLE4VD1CL, maintained only for source compatibility */
    PN_MICROENABLE4VD4CL = 0xa44,                      /**< microEnable IV VD4-CL */
    PN_MICROENABLE4FULLx4 =
        PN_MICROENABLE4VD4CL,                          /**< \deprecated old name for PN_MICROENABLE4VD4CL, maintained only for source compatibility */
    PN_MICROENABLE4AS1CL = 0xa45,                      /**< microEnable IV AS1-CL */
    PN_MICROENABLE4VQ4GE = 0xe44,                      /**< microEnable IV VQ4-GE */
    PN_MICROENABLE4GIGEx4 =
        PN_MICROENABLE4VQ4GE,                          /**< \deprecated old name for PN_MICROENABLE4VQ4GE, maintained only for source compatibility */
    PN_MICROENABLE4AQ4GE = 0xe42,                      /**< microEnable IV AQ4-GE */
    PN_MICROENABLE4_H264CLx1 = 0xb41,                  /**< kappa h264 Fujitsu MB86H51 */
    PN_MICROENABLE4_H264pCLx1 = 0xb42,                 /**< kappa h264 Fujitsu MB86H46A */

    PN_PX100 = 0xc41,                                  /**< PixelPlant PX100 */
    PN_PX200 = 0xc42,                                  /**< PixelPlant PX200 */
    PN_PX210 = 0xc43,                                  /**< PixelPlant PX210-CL */
    PN_PX300 = 0xc44,                                  /**< PixelPlant PX300-CxP */

    PN_MICROENABLE5A1CXP4 = 0xa51,                     /**< microEnable 5 A01-CXP */
    PN_MICROENABLE5A1CLHSF2 = 0xa52,                   /**< microEnable 5 A1-CLHS-F2 */
    PN_MICROENABLE5AQ8CXP6B = 0xa53,                   /**< microEnable 5 AQ8-CXP6B */
    PN_MICROENABLE5AQ8CXP4 =
        PN_MICROENABLE5AQ8CXP6B,                       /**< \deprecated old name for PN_MICROENABLE5AQ8CXP6B, maintained only for source compatibility */
    PN_MICROENABLE5VQ8CXP6B = 0xa54,                   /**< microEnable 5 VQ8-CXP6B */
    PN_MICROENABLE5VQ8CXP4 =
        PN_MICROENABLE5VQ8CXP6B,                       /**<\deprecated old name for PN_MICROENABLE5VQ8CXP6B, maintained only for source compatibility */
    PN_MICROENABLE5AD8CLHSF2 = 0xa55,                  /**< microEnable 5 AD8-CLHS-F2 */
    PN_MICROENABLE5VQ8CXP6D = 0xa56,                   /**< microEnable 5 VQ8-CXP6D */
    PN_MICROENABLE5AQ8CXP6D = 0xa57,                   /**< microEnable 5 AQ8-CXP6D */
    PN_MICROENABLE5VD8CL = 0xa58,                      /**< microEnable 5 VD8-CL */
    PN_MICROENABLE5VF8CL =
        PN_MICROENABLE5VD8CL,                          /**< \deprecated old name for PN_MICROENABLE5VD8CL, maintained only for source compatibility */
    PN_MICROENABLE5A2CLHSF2 = 0xa59,                   /**< microEnable 5 A2-CLHS-F2 */
    PN_MICROENABLE5AD8CL = 0xa5a,                      /**< microEnable 5 AD8-CL */

    PN_MICROENABLE5_LIGHTBRIDGE_VCL_PROTOTYPE = 0x750, /**< LightBridge VCL Prototype */
    PN_MICROENABLE5_LIGHTBRIDGE_MARATHON_VCL = 0x751,  /**< LightBridge/Marathon VCL */
    PN_MICROENABLE5_LIGHTBRIDGE_VCL = 0x7510,          /**< LightBridge VCL */
    PN_MICROENABLE5_MARATHON_VCL = 0x7511,             /**< mE5 marathon VCL */
    PN_MICROENABLE5_MARATHON_AF2_DP = 0x752,           /**< mE5 marathon AF2 (CLHS dual port) */
    PN_MICROENABLE5_MARATHON_ACX_QP = 0x753,           /**< mE5 marathon ACX QP (CXP quad port) */
    PN_MICROENABLE5_LIGHTBRIDGE_MARATHON_ACL = 0x754,  /**< LightBridge/Marathon ACL */
    PN_MICROENABLE5_LIGHTBRIDGE_ACL = 0x7540,          /**< LightBridge ACL */
    PN_MICROENABLE5_MARATHON_ACL = 0x7541,             /**< mE5 marathon ACL */
    PN_MICROENABLE5_MARATHON_ACX_SP = 0x755,           /**< mE5 marathon ACX SP (CXP single port) */
    PN_MICROENABLE5_MARATHON_ACX_DP = 0x756,           /**< mE5 marathon ACX DP (CXP dual port) */
    PN_MICROENABLE5_MARATHON_VCX_QP = 0x757,           /**< mE5 marathon VCX QP (CXP quad port) */
    PN_MICROENABLE5_MARATHON_VF2_DP = 0x758,           /**< mE5 marathon VF2 (CLHS dual port) */
    PN_MICROENABLE5_LIGHTBRIDGE_MARATHON_VCLx = 0x759, /**< LightBridge/Marathon VCLx */
    //PN_MICROENABLE5_LIGHTBRIDGE_VCLx = 0x7590,         /**< LightBridge VCLx */
    PN_MICROENABLE5_MARATHON_VCLx = 0x7591,            /**< mE5 marathon VCLx (VCL with XC7K410T FPGA) */

    PN_TDI = 0xb50,                                    /**< Thunderbolt Device Interface/II */
    PN_TDI_I = 0xb500,                                 /**< Thunderbolt Device Interface */
    PN_TDI_II = 0xb501,                                /**< Thunderbolt Device Interface II*/
    PN_TGATE_USB = 0xb57,                              /**< T-Gate USB/II */
    PN_TGATE_I_USB = 0xb570,                           /**< T-Gate USB/II */
    PN_TGATE_II_USB = 0xb571,                          /**< T-Gate USB/II */
    PN_TGATE = 0xb5e,                                  /**< T-Gate/II */
    PN_TGATE_I = 0xb5e0,                               /**< T-Gate/II */
    PN_TGATE_II = 0xb5e1,                              /**< T-Gate/II */
    PN_TGATE_35 = 0xb58,                               /**< T-Gate/II 35 USB */
    PN_TGATE_I_35 = 0xb580,                            /**< T-Gate/II 35 USB */
    PN_TGATE_II_35 = 0xb581,                           /**< T-Gate/II 35 USB */
    PN_TGATE_35_USB = 0xb59,                           /**< T-Gate/II 35 USB */
    PN_TGATE_I_35_USB = 0xb590,                        /**< T-Gate/II 35 USB */
    PN_TGATE_II_35_USB = 0xb591,                       /**< T-Gate/II 35 USB */
    PN_TTDI = 0xb5f,                                   /**< Test Thunderbolt Device Interface */

    PN_MICROENABLE5_ABACUS_4G_PROTOTYPE = 0xb51,       /**< microEnable 5 Abacus 4G Prototype */
    PN_MICROENABLE5_ABACUS_4G =
        PN_MICROENABLE5_ABACUS_4G_PROTOTYPE,           /**< \deprecated old name for PN_MICROENABLE5_ABACUS_4G_PROTOTYPE, maintained only for source compatibility */
    PN_MICROENABLE5_ABACUS_4G_BASE = 0xb52,            /**< microEnable 5 Abacus 4G Base */
    PN_MICROENABLE5_ABACUS_4G_BASE_II = 0xb53,         /**< microEnable 5 Abacus 4G Base II (7K70T FPGA) */

    PN_MICROENABLE6_KCU105 = 0xA60,                    /**< microEnable 6 Evaluation Board */

#ifdef PLATFORM_PROTOTYPING
    PN_ML605 = 0xff46, /**< Xilinx ML605 Evaluation board */
#endif

    PN_UNKNOWN = 0xffff,
	PN_GENERIC_EVA = 0x10000000,
    PN_NONE = 0
};

#ifdef __cplusplus

// If this file is included from c++, automatically pull in the inline convenience functions
#ifndef SISOBOARDS_INLINE_FUNCTIONS
#define SISOBOARDS_INLINE_FUNCTIONS
#endif

#else // __cplusplus

// If this file is included from pure c, pull in only if requested; then some c++ stuff has to be faked
#ifdef SISOBOARDS_INLINE_FUNCTIONS
typedef enum _bool { false, true } bool;
#define inline
#endif

#endif // __cplusplus

#ifdef SISOBOARDS_INLINE_FUNCTIONS

static inline bool SisoBoardIsMe3(const int boardType)
{
    switch (boardType) {
        case PN_MICROENABLE3I:
        case PN_MICROENABLE3IXXL:
            return true;
        default:
            return false;
    }
}

static inline bool SisoBoardIsMe4(const int boardType)
{
    switch (boardType) {
        case PN_MICROENABLE4AD1CL:
        case PN_MICROENABLE4BASEx4:
        case PN_MICROENABLE4AD4CL:
        case PN_MICROENABLE4VD1CL:
        case PN_MICROENABLE4VD4CL:
        case PN_MICROENABLE4AS1CL:
        case PN_MICROENABLE4_H264CLx1:
        case PN_MICROENABLE4_H264pCLx1:
        case PN_MICROENABLE4VQ4GE:
        case PN_MICROENABLE4AQ4GE:
            return true;
        default:
            return false;
    }
}

static inline bool SisoBoardIsPxp(const int boardType)
{
    switch (boardType) {
        case PN_PX100:
        case PN_PX200:
        case PN_PX210:
        case PN_PX300:
            return true;
        default:
            return false;
    }
}

static inline bool SisoBoardIsMe5(const int boardType)
{
    switch (boardType) {
        case PN_MICROENABLE5A1CLHSF2:
        case PN_MICROENABLE5AD8CLHSF2:
        case PN_MICROENABLE5A2CLHSF2:
        case PN_MICROENABLE5A1CXP4:
        case PN_MICROENABLE5AQ8CXP6B:
        case PN_MICROENABLE5VQ8CXP6B:
        case PN_MICROENABLE5AQ8CXP6D:
        case PN_MICROENABLE5VQ8CXP6D:
        case PN_MICROENABLE5VD8CL:
        case PN_MICROENABLE5AD8CL:
        case PN_TDI:
        case PN_TDI_I:
        case PN_TDI_II:
        case PN_TTDI:
        case PN_MICROENABLE5_ABACUS_4G_PROTOTYPE:
        case PN_MICROENABLE5_ABACUS_4G_BASE:
        case PN_MICROENABLE5_ABACUS_4G_BASE_II:
        case PN_MICROENABLE5_LIGHTBRIDGE_VCL_PROTOTYPE:
        case PN_MICROENABLE5_LIGHTBRIDGE_MARATHON_VCL:
        case PN_MICROENABLE5_LIGHTBRIDGE_VCL:
        case PN_MICROENABLE5_MARATHON_VCL:
        case PN_MICROENABLE5_LIGHTBRIDGE_MARATHON_VCLx:
        case PN_MICROENABLE5_MARATHON_VCLx:
        case PN_MICROENABLE5_MARATHON_AF2_DP:
        case PN_MICROENABLE5_MARATHON_ACX_QP:
        case PN_MICROENABLE5_LIGHTBRIDGE_MARATHON_ACL:
        case PN_MICROENABLE5_LIGHTBRIDGE_ACL:
        case PN_MICROENABLE5_MARATHON_ACL:
        case PN_MICROENABLE5_MARATHON_ACX_SP:
        case PN_MICROENABLE5_MARATHON_ACX_DP:
        case PN_MICROENABLE5_MARATHON_VCX_QP:
        case PN_MICROENABLE5_MARATHON_VF2_DP:
            return true;
        default:
            return false;
    }
}

static inline bool SisoBoardIsMe6(const int boardType)
{
    switch (boardType) {
    case PN_MICROENABLE6_KCU105:
        return true;
    default:
        return false;
    }
}

static inline bool SisoBoardIsAbacus(const int boardType)
{
    switch (boardType) {
    case PN_MICROENABLE5_ABACUS_4G_PROTOTYPE:
    case PN_MICROENABLE5_ABACUS_4G_BASE:
    case PN_MICROENABLE5_ABACUS_4G_BASE_II:
        return true;
    default:
        return false;
    }
}

static inline bool SisoBoardIsIronMan(const int boardType)
{
    switch (boardType) {
    case PN_MICROENABLE5A1CLHSF2:
    case PN_MICROENABLE5AD8CLHSF2:
    case PN_MICROENABLE5A2CLHSF2:
    case PN_MICROENABLE5A1CXP4:
    case PN_MICROENABLE5AQ8CXP6B:
    case PN_MICROENABLE5VQ8CXP6B:
    case PN_MICROENABLE5AQ8CXP6D:
    case PN_MICROENABLE5VQ8CXP6D:
    case PN_MICROENABLE5VD8CL:
    case PN_MICROENABLE5AD8CL:
        return true;
    default:
        return false;
    }
}

static inline bool SisoBoardIsMarathon(const int boardType)
{
    switch (boardType) {
    case PN_MICROENABLE5_LIGHTBRIDGE_VCL_PROTOTYPE:
    case PN_MICROENABLE5_LIGHTBRIDGE_MARATHON_VCL:
    case PN_MICROENABLE5_LIGHTBRIDGE_VCL:
    case PN_MICROENABLE5_MARATHON_VCL:
    case PN_MICROENABLE5_LIGHTBRIDGE_MARATHON_VCLx:
    case PN_MICROENABLE5_MARATHON_VCLx:
    case PN_MICROENABLE5_MARATHON_AF2_DP:
    case PN_MICROENABLE5_MARATHON_ACX_QP:
    case PN_MICROENABLE5_LIGHTBRIDGE_MARATHON_ACL:
    case PN_MICROENABLE5_LIGHTBRIDGE_ACL:
    case PN_MICROENABLE5_MARATHON_ACL:
    case PN_MICROENABLE5_MARATHON_ACX_SP:
    case PN_MICROENABLE5_MARATHON_ACX_DP:
    case PN_MICROENABLE5_MARATHON_VCX_QP:
    case PN_MICROENABLE5_MARATHON_VF2_DP:
        return true;
    default:
        return false;
    }
}

static inline bool SisoBoardIsTdi(const int boardType)
{
    switch (boardType) {
    case PN_TDI:
    case PN_TDI_I:
    case PN_TDI_II:
    case PN_TTDI:
        return true;
    default:
        return false;
    }
}

static inline bool SisoBoardIsExternal(const int boardType)
{
    switch (boardType) {
    case PN_MICROENABLE5_LIGHTBRIDGE_VCL_PROTOTYPE:
    case PN_MICROENABLE5_LIGHTBRIDGE_MARATHON_VCL:
    case PN_MICROENABLE5_LIGHTBRIDGE_VCL:
    case PN_MICROENABLE5_LIGHTBRIDGE_MARATHON_ACL:
    case PN_MICROENABLE5_LIGHTBRIDGE_ACL:
    case PN_TDI:
    case PN_TDI_I:
    case PN_TDI_II:
    case PN_TTDI:
    case PN_TGATE:
    case PN_TGATE_I:
    case PN_TGATE_II:
    case PN_TGATE_USB:
    case PN_TGATE_I_USB:
    case PN_TGATE_II_USB:
    case PN_TGATE_35:
    case PN_TGATE_I_35:
    case PN_TGATE_II_35:
    case PN_TGATE_35_USB:
    case PN_TGATE_I_35_USB:
    case PN_TGATE_II_35_USB:
        return true;
    default:
        return false;
    }
}

static inline bool SisoBoardIsCL(const int boardType)
{
    switch (boardType) {
        case PN_MICROENABLE4AD1CL:
        case PN_MICROENABLE4BASEx4:
        case PN_MICROENABLE4AD4CL:
        case PN_MICROENABLE4VD1CL:
        case PN_MICROENABLE4VD4CL:
        case PN_MICROENABLE4AS1CL:
        case PN_MICROENABLE5VD8CL:
        case PN_MICROENABLE5AD8CL:
        case PN_MICROENABLE5_LIGHTBRIDGE_VCL_PROTOTYPE:
        case PN_MICROENABLE5_LIGHTBRIDGE_MARATHON_VCL:
        case PN_MICROENABLE5_LIGHTBRIDGE_VCL:
        case PN_MICROENABLE5_MARATHON_VCL:
        case PN_MICROENABLE5_LIGHTBRIDGE_MARATHON_VCLx:
        case PN_MICROENABLE5_MARATHON_VCLx:
        case PN_MICROENABLE5_LIGHTBRIDGE_MARATHON_ACL:
        case PN_MICROENABLE5_LIGHTBRIDGE_ACL:
        case PN_MICROENABLE5_MARATHON_ACL:
            return true;
        default:
            return false;
    }
}

static inline bool SisoBoardIsGigE(const int boardType)
{
    switch (boardType) {
        case PN_MICROENABLE4VQ4GE:
        case PN_MICROENABLE4AQ4GE:
        case PN_MICROENABLE5_ABACUS_4G_PROTOTYPE:
        case PN_MICROENABLE5_ABACUS_4G_BASE:
        case PN_MICROENABLE5_ABACUS_4G_BASE_II:
            return true;
        default:
            return false;
    }
}

static inline bool SisoBoardIsCXP(const int boardType)
{
    switch (boardType) {
        case PN_MICROENABLE5A1CXP4:
        case PN_MICROENABLE5AQ8CXP6B:
        case PN_MICROENABLE5VQ8CXP6B:
        case PN_MICROENABLE5AQ8CXP6D:
        case PN_MICROENABLE5VQ8CXP6D:
        case PN_MICROENABLE5_MARATHON_ACX_QP:
        case PN_MICROENABLE5_MARATHON_ACX_SP:
        case PN_MICROENABLE5_MARATHON_ACX_DP:
        case PN_MICROENABLE5_MARATHON_VCX_QP:
            return true;
        default:
            return false;
    }
}

static inline bool SisoBoardIsCLHS(const int boardType)
{
    switch (boardType) {
        case PN_MICROENABLE5A1CLHSF2:
        case PN_MICROENABLE5AD8CLHSF2:
        case PN_MICROENABLE5A2CLHSF2:
        case PN_MICROENABLE5_MARATHON_AF2_DP:
        case PN_MICROENABLE5_MARATHON_VF2_DP:
            return true;
        default:
            return false;
    }
}

static inline bool SisoBoardIsV(const int boardType)
{
    switch (boardType) {
    case PN_MICROENABLE4VD1CL:
    case PN_MICROENABLE4VD4CL:
    case PN_MICROENABLE4VQ4GE:
    case PN_MICROENABLE5VD8CL:
    case PN_MICROENABLE5VQ8CXP6B:
    case PN_MICROENABLE5VQ8CXP6D:
    case PN_MICROENABLE5_LIGHTBRIDGE_VCL_PROTOTYPE:
    case PN_MICROENABLE5_LIGHTBRIDGE_MARATHON_VCL:
    case PN_MICROENABLE5_LIGHTBRIDGE_VCL:
    case PN_MICROENABLE5_MARATHON_VCL:
    case PN_MICROENABLE5_LIGHTBRIDGE_MARATHON_VCLx:
    case PN_MICROENABLE5_MARATHON_VCLx:
    case PN_MICROENABLE5_MARATHON_VCX_QP:
    case PN_MICROENABLE5_MARATHON_VF2_DP:
        return true;
    default:
        return false;
    }
}

static inline int SisoBoardNumberOfPhysicalPorts(const int boardType)
{
    switch (boardType) {
    case PN_MICROENABLE4AS1CL:
    case PN_MICROENABLE5_MARATHON_ACX_SP:
        return 1;
    case PN_MICROENABLE4AD1CL:
    case PN_MICROENABLE4BASEx4:
    case PN_MICROENABLE4AD4CL:
    case PN_MICROENABLE4VD1CL:
    case PN_MICROENABLE4VD4CL:
    case PN_MICROENABLE5VD8CL:
    case PN_MICROENABLE5AD8CL:
    case PN_MICROENABLE5A1CLHSF2:
    case PN_MICROENABLE5AD8CLHSF2:
    case PN_MICROENABLE5A2CLHSF2:
    case PN_MICROENABLE5_LIGHTBRIDGE_VCL_PROTOTYPE:
    case PN_MICROENABLE5_LIGHTBRIDGE_MARATHON_VCL:
    case PN_MICROENABLE5_LIGHTBRIDGE_VCL:
    case PN_MICROENABLE5_MARATHON_VCL:
    case PN_MICROENABLE5_LIGHTBRIDGE_MARATHON_VCLx:
    case PN_MICROENABLE5_MARATHON_VCLx:
    case PN_MICROENABLE5_LIGHTBRIDGE_MARATHON_ACL:
    case PN_MICROENABLE5_LIGHTBRIDGE_ACL:
    case PN_MICROENABLE5_MARATHON_ACL:
    case PN_MICROENABLE5_MARATHON_ACX_DP:
    case PN_MICROENABLE5_MARATHON_AF2_DP:
    case PN_MICROENABLE5_MARATHON_VF2_DP:
        return 2;
    case PN_MICROENABLE4VQ4GE:
    case PN_MICROENABLE4AQ4GE:
    case PN_MICROENABLE5A1CXP4:
    case PN_MICROENABLE5AQ8CXP6B:
    case PN_MICROENABLE5VQ8CXP6B:
    case PN_MICROENABLE5AQ8CXP6D:
    case PN_MICROENABLE5VQ8CXP6D:
    case PN_MICROENABLE5_MARATHON_ACX_QP:
    case PN_MICROENABLE5_MARATHON_VCX_QP:
    case PN_MICROENABLE5_ABACUS_4G_PROTOTYPE:
    case PN_MICROENABLE5_ABACUS_4G_BASE:
    case PN_MICROENABLE5_ABACUS_4G_BASE_II:
        return 4;
    default:
        return 0;
    }
}

static inline int SisoBoardNumberOfPCIeLanes(const int boardType)
{
    switch (boardType) {
    case PN_MICROENABLE4AS1CL:
    case PN_MICROENABLE4AD1CL:
    case PN_MICROENABLE4VD1CL:
        return 1;
    case PN_MICROENABLE4BASEx4:
    case PN_MICROENABLE4AD4CL:
    case PN_MICROENABLE4VD4CL:
    case PN_MICROENABLE4VQ4GE:
    case PN_MICROENABLE4AQ4GE:
    case PN_MICROENABLE5_LIGHTBRIDGE_VCL_PROTOTYPE:
    case PN_MICROENABLE5_LIGHTBRIDGE_MARATHON_VCL:
    case PN_MICROENABLE5_LIGHTBRIDGE_VCL:
    case PN_MICROENABLE5_MARATHON_VCL:
    case PN_MICROENABLE5_LIGHTBRIDGE_MARATHON_VCLx:
    case PN_MICROENABLE5_MARATHON_VCLx:
    case PN_MICROENABLE5_LIGHTBRIDGE_MARATHON_ACL:
    case PN_MICROENABLE5_LIGHTBRIDGE_ACL:
    case PN_MICROENABLE5_MARATHON_ACL:
    case PN_MICROENABLE5_MARATHON_ACX_DP:
    case PN_MICROENABLE5_MARATHON_ACX_SP:
    case PN_MICROENABLE5_MARATHON_ACX_QP:
    case PN_MICROENABLE5_MARATHON_VCX_QP:
    case PN_MICROENABLE5_MARATHON_AF2_DP:
    case PN_MICROENABLE5_MARATHON_VF2_DP:
    case PN_MICROENABLE5_ABACUS_4G_PROTOTYPE:
    case PN_MICROENABLE5_ABACUS_4G_BASE:
    case PN_MICROENABLE5_ABACUS_4G_BASE_II:
        return 4;
    case PN_MICROENABLE5VD8CL:
    case PN_MICROENABLE5AD8CL:
    case PN_MICROENABLE5A1CLHSF2:
    case PN_MICROENABLE5AD8CLHSF2:
    case PN_MICROENABLE5A2CLHSF2:
    case PN_MICROENABLE5A1CXP4:
    case PN_MICROENABLE5AQ8CXP6B:
    case PN_MICROENABLE5VQ8CXP6B:
    case PN_MICROENABLE5AQ8CXP6D:
    case PN_MICROENABLE5VQ8CXP6D:
        return 8;
    default:
        return 0;
    }
}

static inline int SisoBoardPCIeGeneration(const int boardType)
{
    switch (boardType) {
    case PN_MICROENABLE4AS1CL:
    case PN_MICROENABLE4AD1CL:
    case PN_MICROENABLE4VD1CL:
    case PN_MICROENABLE4BASEx4:
    case PN_MICROENABLE4AD4CL:
    case PN_MICROENABLE4VD4CL:
    case PN_MICROENABLE4VQ4GE:
    case PN_MICROENABLE4AQ4GE:
        return 1;
    case PN_MICROENABLE5VD8CL:
    case PN_MICROENABLE5AD8CL:
    case PN_MICROENABLE5A1CLHSF2:
    case PN_MICROENABLE5AD8CLHSF2:
    case PN_MICROENABLE5A2CLHSF2:
    case PN_MICROENABLE5A1CXP4:
    case PN_MICROENABLE5AQ8CXP6B:
    case PN_MICROENABLE5VQ8CXP6B:
    case PN_MICROENABLE5AQ8CXP6D:
    case PN_MICROENABLE5VQ8CXP6D:
    case PN_MICROENABLE5_LIGHTBRIDGE_VCL_PROTOTYPE:
    case PN_MICROENABLE5_LIGHTBRIDGE_MARATHON_VCL:
    case PN_MICROENABLE5_LIGHTBRIDGE_VCL:
    case PN_MICROENABLE5_MARATHON_VCL:
    case PN_MICROENABLE5_LIGHTBRIDGE_MARATHON_VCLx:
    case PN_MICROENABLE5_MARATHON_VCLx:
    case PN_MICROENABLE5_LIGHTBRIDGE_MARATHON_ACL:
    case PN_MICROENABLE5_LIGHTBRIDGE_ACL:
    case PN_MICROENABLE5_MARATHON_ACL:
    case PN_MICROENABLE5_MARATHON_ACX_DP:
    case PN_MICROENABLE5_MARATHON_ACX_SP:
    case PN_MICROENABLE5_MARATHON_ACX_QP:
    case PN_MICROENABLE5_MARATHON_VCX_QP:
    case PN_MICROENABLE5_MARATHON_AF2_DP:
    case PN_MICROENABLE5_MARATHON_VF2_DP:
    case PN_MICROENABLE5_ABACUS_4G_PROTOTYPE:
    case PN_MICROENABLE5_ABACUS_4G_BASE:
    case PN_MICROENABLE5_ABACUS_4G_BASE_II:
        return 2;
    default:
        return 0;
    }
}

static inline bool SisoBoardIsOEM(const int boardType)
{
    switch (boardType) {
    case PN_TDI:
    case PN_TDI_I:
    case PN_TDI_II:
    case PN_TTDI:
    case PN_TGATE:
    case PN_TGATE_I:
    case PN_TGATE_II:
    case PN_TGATE_USB:
    case PN_TGATE_I_USB:
    case PN_TGATE_II_USB:
    case PN_TGATE_35:
    case PN_TGATE_I_35:
    case PN_TGATE_II_35:
    case PN_TGATE_35_USB:
    case PN_TGATE_I_35_USB:
    case PN_TGATE_II_35_USB:
    case PN_MICROENABLE5_ABACUS_4G_PROTOTYPE:
    case PN_MICROENABLE5_ABACUS_4G_BASE:
    case PN_MICROENABLE5_ABACUS_4G_BASE_II:
        return true;
    default:
        return false;
    }
}

static inline bool SisoBoardHasExtendedType(const int boardType)
{
    switch (boardType) {
    case PN_MICROENABLE5_LIGHTBRIDGE_MARATHON_ACL:
    case PN_MICROENABLE5_LIGHTBRIDGE_MARATHON_VCL:
    case PN_MICROENABLE5_LIGHTBRIDGE_ACL:
    case PN_MICROENABLE5_MARATHON_ACL:
    case PN_MICROENABLE5_LIGHTBRIDGE_VCL:
    case PN_MICROENABLE5_MARATHON_VCL:
    case PN_MICROENABLE5_LIGHTBRIDGE_MARATHON_VCLx:
    case PN_MICROENABLE5_MARATHON_VCLx:
    case PN_TDI:
    case PN_TDI_I:
    case PN_TDI_II:
    case PN_TGATE:
    case PN_TGATE_I:
    case PN_TGATE_II:
    case PN_TGATE_USB:
    case PN_TGATE_I_USB:
    case PN_TGATE_II_USB:
    case PN_TGATE_35:
    case PN_TGATE_I_35:
    case PN_TGATE_II_35:
    case PN_TGATE_35_USB:
    case PN_TGATE_I_35_USB:
    case PN_TGATE_II_35_USB:
        return true;
    default:
        return false;
    }
}

static inline enum siso_board_type SisoBoardTypeFromSerialNumber(unsigned int serial)
{
    enum siso_board_type boardType = (enum siso_board_type) ((serial >> 20) & 0xfff);
    if (boardType == PN_MICROENABLE5_LIGHTBRIDGE_MARATHON_ACL ||
        boardType == PN_MICROENABLE5_LIGHTBRIDGE_MARATHON_VCL ||
        boardType == PN_MICROENABLE5_LIGHTBRIDGE_MARATHON_VCLx ||
        boardType == PN_TDI ||
        boardType == PN_TGATE ||
        boardType == PN_TGATE_USB ||
        boardType == PN_TGATE_35 ||
        boardType == PN_TGATE_35_USB) {
            boardType = (enum siso_board_type) ((serial >> 16) & 0xffff);
    }

    return boardType;
};

#endif

#endif
