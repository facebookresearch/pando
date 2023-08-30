//-----------------------------------------------------------------------------
//  Basler pylon SDK
//  Copyright (c) 2019-2021 Basler AG
//  http://www.baslerweb.com
//-----------------------------------------------------------------------------

/*!
\file
\brief A parameter class containing all parameters as members that are available for pylon device transport layers

The parameter class is used by the \c Pylon::CBaslerUniversalInstantCamera class.
The \ref sample_ParametrizeCamera_NativeParameterAccess code sample shows how to access camera parameters via the \c Pylon::CBaslerUniversalInstantCamera class.
*/

//-----------------------------------------------------------------------------
//  This file is generated automatically
//  Do not modify!
//-----------------------------------------------------------------------------

#ifndef BASLER_PYLON_UNIVERSALTLPARAMS_H
#define BASLER_PYLON_UNIVERSALTLPARAMS_H

#pragma once

// common parameter types
#include <pylon/ParameterIncludes.h>
#include <pylon/EnumParameterT.h>

namespace Basler_UniversalTLParams
{
    //**************************************************************************************************
    // Enumerations
    //**************************************************************************************************
    //! Valid values for AreaTriggerMode
    enum AreaTriggerModeEnums
    {
        AreaTriggerMode_External,  //!< The trigger mode is set to External - Applies to: CoaXPress
        AreaTriggerMode_Generator,  //!< The trigger mode is set to Generator - Applies to: CoaXPress
        AreaTriggerMode_Software,  //!< The trigger mode is set to Software - Applies to: CoaXPress
        AreaTriggerMode_Synchronized  //!< The trigger mode is set to Synchronized - Applies to: CoaXPress
    };

    //! Valid values for BitAlignment
    enum BitAlignmentEnums
    {
        BitAlignment_CustomBitShift,  //!< The bits are shifted by a user-defined value - Applies to: CoaXPress
        BitAlignment_LeftAligned,  //!< The bits left-aligned   - Applies to: CoaXPress
        BitAlignment_RightAligned  //!< The bits right-aligned   - Applies to: CoaXPress
    };

    //! Valid values for CxpLinkConfiguration
    enum CxpLinkConfigurationEnums
    {
        CxpLinkConfiguration_Auto,  //!< Link speed is controlled automatically - Applies to: CoaXPress
        CxpLinkConfiguration_CXP10_X1,  //!< 1 connection is operating at CXP-10 speed (10 0 Gbps) - Applies to: CoaXPress
        CxpLinkConfiguration_CXP10_X2,  //!< 2 connections are operating at CXP-10 speed (10 0 Gbps) - Applies to: CoaXPress
        CxpLinkConfiguration_CXP10_X3,  //!< 3 connections are operating at CXP-10 speed (10 0 Gbps) - Applies to: CoaXPress
        CxpLinkConfiguration_CXP10_X4,  //!< 4 connections are operating at CXP-10 speed (10 0 Gbps) - Applies to: CoaXPress
        CxpLinkConfiguration_CXP12_X1,  //!< 1 connection is operating at CXP-12 speed (12 50 Gbps) - Applies to: CoaXPress
        CxpLinkConfiguration_CXP12_X2,  //!< 2 connections are operating at CXP-12 speed (12 50 Gbps) - Applies to: CoaXPress
        CxpLinkConfiguration_CXP12_X3,  //!< 3 connections are operating at CXP-12 speed (12 50 Gbps) - Applies to: CoaXPress
        CxpLinkConfiguration_CXP12_X4,  //!< 4 connections are operating at CXP-12 speed (12 50 Gbps) - Applies to: CoaXPress
        CxpLinkConfiguration_CXP1_X1,  //!< 1 connection is operating at CXP-1 speed (1 25 Gbps) - Applies to: CoaXPress
        CxpLinkConfiguration_CXP1_X2,  //!< 2 connections are operating at CXP-1 speed (1 25 Gbps) - Applies to: CoaXPress
        CxpLinkConfiguration_CXP1_X3,  //!< 3 connections are operating at CXP-1 speed (1 25 Gbps) - Applies to: CoaXPress
        CxpLinkConfiguration_CXP1_X4,  //!< 4 connections are operating at CXP-1 speed (1 25 Gbps) - Applies to: CoaXPress
        CxpLinkConfiguration_CXP2_X1,  //!< 1 connection is operating at CXP-2 speed (2 50 Gbps) - Applies to: CoaXPress
        CxpLinkConfiguration_CXP2_X2,  //!< 2 connections are operating at CXP-2 speed (2 50 Gbps) - Applies to: CoaXPress
        CxpLinkConfiguration_CXP2_X3,  //!< 3 connections are operating at CXP-2 speed (2 50 Gbps) - Applies to: CoaXPress
        CxpLinkConfiguration_CXP2_X4,  //!< 4 connections are operating at CXP-2 speed (2 50 Gbps) - Applies to: CoaXPress
        CxpLinkConfiguration_CXP3_X1,  //!< 1 connection is operating at CXP-3 speed (3 125 Gbps) - Applies to: CoaXPress
        CxpLinkConfiguration_CXP3_X2,  //!< 2 connections are operating at CXP-3 speed (3 125 Gbps) - Applies to: CoaXPress
        CxpLinkConfiguration_CXP3_X3,  //!< 3 connections are operating at CXP-3 speed (3 125 Gbps) - Applies to: CoaXPress
        CxpLinkConfiguration_CXP3_X4,  //!< 4 connections are operating at CXP-3 speed (3 125 Gbps) - Applies to: CoaXPress
        CxpLinkConfiguration_CXP5_X1,  //!< 1 connection is operating at CXP-5 speed (5 00 Gbps) - Applies to: CoaXPress
        CxpLinkConfiguration_CXP5_X2,  //!< 2 connections are operating at CXP-5 speed (5 00 Gbps) - Applies to: CoaXPress
        CxpLinkConfiguration_CXP5_X3,  //!< 3 connections are operating at CXP-5 speed (5 00 Gbps) - Applies to: CoaXPress
        CxpLinkConfiguration_CXP5_X4,  //!< 4 connections are operating at CXP-5 speed (5 00 Gbps) - Applies to: CoaXPress
        CxpLinkConfiguration_CXP6_X1,  //!< 1 connection is operating at CXP-6 speed (6 25 Gbps) - Applies to: CoaXPress
        CxpLinkConfiguration_CXP6_X2,  //!< 2 connections are operating at CXP-6 speed (6 25 Gbps) - Applies to: CoaXPress
        CxpLinkConfiguration_CXP6_X3,  //!< 3 connections are operating at CXP-6 speed (6 25 Gbps) - Applies to: CoaXPress
        CxpLinkConfiguration_CXP6_X4  //!< 4 connections are operating at CXP-6 speed (6 25 Gbps) - Applies to: CoaXPress
    };

    //! Valid values for CxpTriggerPacketMode
    enum CxpTriggerPacketModeEnums
    {
        CxpTriggerPacketMode_CXPTriggerRising,  //!< Only rising edge trigger packets are sent - Applies to: CoaXPress
        CxpTriggerPacketMode_CXPTriggerStandard  //!< Rising and falling edge trigger packets are sent on an alternating basis - Applies to: CoaXPress
    };

    //! Valid values for DeviceAccessStatus
    enum DeviceAccessStatusEnums
    {
        DeviceAccessStatus_Busy,  //!< The device is already opened by another entity - Applies to: CoaXPress and blaze
        DeviceAccessStatus_NoAccess,  //!< You can't connect to the device - Applies to: CoaXPress and blaze
        DeviceAccessStatus_OpenReadOnly,  //!< The device is opened in Read-only mode by this GenTL host - Applies to: CoaXPress and blaze
        DeviceAccessStatus_OpenReadWrite,  //!< The device is opened in Read/Write mode by this GenTL host - Applies to: CoaXPress and blaze
        DeviceAccessStatus_ReadOnly,  //!< The device offers read-only access - Applies to: CoaXPress and blaze
        DeviceAccessStatus_ReadWrite,  //!< The device offers read/write access - Applies to: CoaXPress and blaze
        DeviceAccessStatus_Unknown  //!< The status is unknown - Applies to: CoaXPress and blaze
    };

    //! Valid values for DeviceEndianessMechanism
    enum DeviceEndianessMechanismEnums
    {
        DeviceEndianessMechanism_Legacy,  //!< The device endianness is handled according to GenICam Schema 1 0 - Applies to: blaze
        DeviceEndianessMechanism_Standard  //!< The device endianness is handled according to GenICam Schema 1 1 and later - Applies to: blaze
    };

    //! Valid values for DeviceType
    enum DeviceTypeEnums
    {
        DeviceType_CL,  //!< The device uses the Camera Link transport layer - Applies to: CoaXPress
        DeviceType_CLHS,  //!< The device uses the Camera Link HS transport layer - Applies to: CoaXPress
        DeviceType_CXP,  //!< The device uses the CoaXPress transport layer - Applies to: CoaXPress
        DeviceType_CameraLink,  //!< Camera Link - Applies to: CoaXPress
        DeviceType_CameraLinkHS,  //!< Camera Link High Speed - Applies to: CoaXPress
        DeviceType_CoaXPress,  //!< CoaXPress - Applies to: CoaXPress
        DeviceType_Custom,  //!< The device uses a custom transport layer - Applies to: CoaXPress
        DeviceType_GEV,  //!< The device uses the GigE Vision transport layer - Applies to: CoaXPress
        DeviceType_GigEVision,  //!< GigE Vision - Applies to: CoaXPress
        DeviceType_Mixed,  //!< Different interface modules of the GenTL Producer are of different types - Applies to: CoaXPress
        DeviceType_U3V,  //!< The device uses the USB3 Vision transport layer - Applies to: CoaXPress
        DeviceType_USB3Vision  //!< USB3 Vision - Applies to: CoaXPress
    };

    //! Valid values for EventNotification
    enum EventNotificationEnums
    {
        EventNotification_Off,  //!< The selected Event notification is disabled - Applies to: CoaXPress and blaze
        EventNotification_On,  //!< The selected Event notification is enabled - Applies to: CoaXPress and blaze
        EventNotification_Once  //!< The selected Event notification is enabled for one event then return to Off state - Applies to: CoaXPress and blaze
    };

    //! Valid values for EventSelector
    enum EventSelectorEnums
    {
        EventSelector_DeviceLost  //!< The Device Lost event is selected - Applies to: CoaXPress and blaze
    };

    //! Valid values for Format
    enum FormatEnums
    {
        Format_BGR10p,  //!< The pixel output format is set to BGR10p - Applies to: CoaXPress
        Format_BGR12p,  //!< The pixel output format is set to BGR12p - Applies to: CoaXPress
        Format_BGR14p,  //!< The pixel output format is set to BGR14p - Applies to: CoaXPress
        Format_BGR16,  //!< The pixel output format is set to BGR16 - Applies to: CoaXPress
        Format_BGR8,  //!< The pixel output format is set to BGR8 - Applies to: CoaXPress
        Format_BGRa8,  //!< The pixel output format is set to BGRa8 - Applies to: CoaXPress
        Format_BayerBG10p,  //!< The pixel output format is set to BayerBG10p - Applies to: CoaXPress
        Format_BayerBG12p,  //!< The pixel output format is set to BayerBG12p - Applies to: CoaXPress
        Format_BayerBG14p,  //!< The pixel output format is set to BayerBG14p - Applies to: CoaXPress
        Format_BayerBG16,  //!< The pixel output format is set to BayerBG16 - Applies to: CoaXPress
        Format_BayerBG8,  //!< The pixel output format is set to BayerBG8 - Applies to: CoaXPress
        Format_BayerGB10p,  //!< The pixel output format is set to BayerGB10p - Applies to: CoaXPress
        Format_BayerGB12p,  //!< The pixel output format is set to BayerGB12p - Applies to: CoaXPress
        Format_BayerGB14p,  //!< The pixel output format is set to BayerGB14p - Applies to: CoaXPress
        Format_BayerGB16,  //!< The pixel output format is set to BayerGB16 - Applies to: CoaXPress
        Format_BayerGB8,  //!< The pixel output format is set to BayerGB8 - Applies to: CoaXPress
        Format_BayerGR10p,  //!< The pixel output format is set to BayerGR10p - Applies to: CoaXPress
        Format_BayerGR12p,  //!< The pixel output format is set to BayerGR12p - Applies to: CoaXPress
        Format_BayerGR14p,  //!< The pixel output format is set to BayerGR14p - Applies to: CoaXPress
        Format_BayerGR16,  //!< The pixel output format is set to BayerGR16 - Applies to: CoaXPress
        Format_BayerGR8,  //!< The pixel output format is set to BayerGR8 - Applies to: CoaXPress
        Format_BayerRG10p,  //!< The pixel output format is set to BayerRG10p - Applies to: CoaXPress
        Format_BayerRG12p,  //!< The pixel output format is set to BayerRG12p - Applies to: CoaXPress
        Format_BayerRG14p,  //!< The pixel output format is set to BayerRG14p - Applies to: CoaXPress
        Format_BayerRG16,  //!< The pixel output format is set to BayerRG16 - Applies to: CoaXPress
        Format_BayerRG8,  //!< The pixel output format is set to BayerRG8 - Applies to: CoaXPress
        Format_Mono10p,  //!< The pixel output format is set to Mono10p - Applies to: CoaXPress
        Format_Mono12p,  //!< The pixel output format is set to Mono12p - Applies to: CoaXPress
        Format_Mono14p,  //!< The pixel output format is set to Mono14p - Applies to: CoaXPress
        Format_Mono16,  //!< The pixel output format is set to Mono16 - Applies to: CoaXPress
        Format_Mono8,  //!< The pixel output format is set to Mono8 - Applies to: CoaXPress
        Format_RGB10p,  //!< The pixel output format is set to RGB10p - Applies to: CoaXPress
        Format_RGB12p,  //!< The pixel output format is set to RGB12p - Applies to: CoaXPress
        Format_RGB14p,  //!< The pixel output format is set to RGB14p - Applies to: CoaXPress
        Format_RGB16,  //!< The pixel output format is set to RGB16 - Applies to: CoaXPress
        Format_RGB8,  //!< The pixel output format is set to RGB8 - Applies to: CoaXPress
        Format_RGBa8,  //!< The pixel output format is set to RGBa8 - Applies to: CoaXPress
        Format_YCbCr422_8  //!< The pixel output format is set to YCbCr422_8 - Applies to: CoaXPress
    };

    //! Valid values for LutEnable
    enum LutEnableEnums
    {
        LutEnable_Off,  //!< Off - Applies to: CoaXPress
        LutEnable_On  //!< On - Applies to: CoaXPress
    };

    //! Valid values for LutImplementationType
    enum LutImplementationTypeEnums
    {
        LutImplementationType_FullLUT,  //!< Defines 1 value for each possible pixel value - Applies to: CoaXPress
        LutImplementationType_KneeLUT  //!< Uses an interpolation for pixel values based one a set of defined values - Applies to: CoaXPress
    };

    //! Valid values for LutType
    enum LutTypeEnums
    {
        LutType_Processor,  //!< Processor - Applies to: CoaXPress
        LutType_UserFile  //!< Use a user file to configure the value - Applies to: CoaXPress
    };

    //! Valid values for MissingCameraFrameResponse
    enum MissingCameraFrameResponseEnums
    {
        MissingCameraFrameResponse_No,  //!< The camera sends a frame for each output trigger pulse - Applies to: CoaXPress
        MissingCameraFrameResponse_Yes  //!< The camera doesn't send a frame for each output trigger pulse - Applies to: CoaXPress
    };

    //! Valid values for PixelFormat
    enum PixelFormatEnums
    {
        PixelFormat_BayerBG10p,  //!< The pixel format is set to BayerBG10p - Applies to: CoaXPress
        PixelFormat_BayerBG12p,  //!< The pixel format is set to BayerBG12p - Applies to: CoaXPress
        PixelFormat_BayerBG14p,  //!< The pixel format is set to BayerBG14p - Applies to: CoaXPress
        PixelFormat_BayerBG8,  //!< The pixel format is set to BayerBG8 - Applies to: CoaXPress
        PixelFormat_BayerGB10p,  //!< The pixel format is set to BayerGB10p - Applies to: CoaXPress
        PixelFormat_BayerGB12p,  //!< The pixel format is set to BayerGB12p - Applies to: CoaXPress
        PixelFormat_BayerGB14p,  //!< The pixel format is set to BayerGB14p - Applies to: CoaXPress
        PixelFormat_BayerGB8,  //!< The pixel format is set to BayerGB8 - Applies to: CoaXPress
        PixelFormat_BayerGR10p,  //!< The pixel format is set to BayerGR10p - Applies to: CoaXPress
        PixelFormat_BayerGR12p,  //!< The pixel format is set to BayerGR12p - Applies to: CoaXPress
        PixelFormat_BayerGR14p,  //!< The pixel format is set to BayerGR14p - Applies to: CoaXPress
        PixelFormat_BayerGR8,  //!< The pixel format is set to BayerGR8 - Applies to: CoaXPress
        PixelFormat_BayerRG10p,  //!< The pixel format is set to BayerRG10p - Applies to: CoaXPress
        PixelFormat_BayerRG12p,  //!< The pixel format is set to BayerRG12p - Applies to: CoaXPress
        PixelFormat_BayerRG14p,  //!< The pixel format is set to BayerRG14p - Applies to: CoaXPress
        PixelFormat_BayerRG8,  //!< The pixel format is set to BayerRG8 - Applies to: CoaXPress
        PixelFormat_Mono10,  //!< The pixel format is set to Mono10 - Applies to: CoaXPress
        PixelFormat_Mono10p,  //!< The pixel output format is set to Mono10p - Applies to: CoaXPress
        PixelFormat_Mono12,  //!< The pixel format is set to Mono12 - Applies to: CoaXPress
        PixelFormat_Mono12p,  //!< The pixel output format is set to Mono12p - Applies to: CoaXPress
        PixelFormat_Mono14p,  //!< The pixel output format is set to Mono14p - Applies to: CoaXPress
        PixelFormat_Mono16,  //!< The pixel format is set to Mono16 - Applies to: CoaXPress
        PixelFormat_Mono8,  //!< The pixel format is set to Mono8 - Applies to: CoaXPress
        PixelFormat_RGB10p,  //!< The pixel format is set to RGB10p - Applies to: CoaXPress
        PixelFormat_RGB12p,  //!< The pixel format is set to RGB12p - Applies to: CoaXPress
        PixelFormat_RGB14p,  //!< The pixel format is set to RGB14p - Applies to: CoaXPress
        PixelFormat_RGB16,  //!< The pixel format is set to RGB16 - Applies to: CoaXPress
        PixelFormat_RGB8,  //!< The pixel format is set to RGB8 - Applies to: CoaXPress
        PixelFormat_YCbCr422_8  //!< The pixel format is set to YCbCr422_8 - Applies to: CoaXPress
    };

    //! Valid values for ProcessingInvert
    enum ProcessingInvertEnums
    {
        ProcessingInvert_Off,  //!< Off - Applies to: CoaXPress
        ProcessingInvert_On  //!< On - Applies to: CoaXPress
    };

    //! Valid values for SoftwareTriggerIsBusy
    enum SoftwareTriggerIsBusyEnums
    {
        SoftwareTriggerIsBusy_Busy,  //!< The software trigger is busy - Applies to: CoaXPress
        SoftwareTriggerIsBusy_NotBusy  //!< The software trigger is not busy - Applies to: CoaXPress
    };

    //! Valid values for SystemmonitorByteAlignment8b10bLocked
    enum SystemmonitorByteAlignment8b10bLockedEnums
    {
        SystemmonitorByteAlignment8b10bLocked_No,  //!< No - Applies to: CoaXPress
        SystemmonitorByteAlignment8b10bLocked_Yes  //!< Yes - Applies to: CoaXPress
    };

    //! Valid values for SystemmonitorExternalPower
    enum SystemmonitorExternalPowerEnums
    {
        SystemmonitorExternalPower_NoPower,  //!< No power - Applies to: CoaXPress
        SystemmonitorExternalPower_PowerGood  //!< Power is ok - Applies to: CoaXPress
    };

    //! Valid values for SystemmonitorPowerOverCxpState
    enum SystemmonitorPowerOverCxpStateEnums
    {
        SystemmonitorPowerOverCxpState_PoCXPADCChipError,  //!< ADC Chip Error - Applies to: CoaXPress
        SystemmonitorPowerOverCxpState_PoCXPBooting,  //!< Booting, not initalized - Applies to: CoaXPress
        SystemmonitorPowerOverCxpState_PoCXPDisabled,  //!< No Power over CXP - Applies to: CoaXPress
        SystemmonitorPowerOverCxpState_PoCXPLowVolt,  //!< Low voltage - Applies to: CoaXPress
        SystemmonitorPowerOverCxpState_PoCXPMaxCurrent,  //!< Maximum current - Applies to: CoaXPress
        SystemmonitorPowerOverCxpState_PoCXPMinCurrent,  //!< Minimum current - Applies to: CoaXPress
        SystemmonitorPowerOverCxpState_PoCXPNotConnected,  //!< No cable connected - Applies to: CoaXPress
        SystemmonitorPowerOverCxpState_PoCXPOK,  //!< Power over CXP OK - Applies to: CoaXPress
        SystemmonitorPowerOverCxpState_PoCXPOverVolt  //!< Over voltage - Applies to: CoaXPress
    };

    //! Valid values for TriggerCameraOutSelect
    enum TriggerCameraOutSelectEnums
    {
        TriggerCameraOutSelect_BypassFrontGPI0,  //!< The output source is set to BypassFrontGPI0 - Applies to: CoaXPress
        TriggerCameraOutSelect_BypassFrontGPI1,  //!< The output source is set to BypassFrontGPI1 - Applies to: CoaXPress
        TriggerCameraOutSelect_BypassFrontGPI2,  //!< The output source is set to BypassFrontGPI2 - Applies to: CoaXPress
        TriggerCameraOutSelect_BypassFrontGPI3,  //!< The output source is set to BypassFrontGPI3 - Applies to: CoaXPress
        TriggerCameraOutSelect_BypassGPI0,  //!< Bypass the GPI 0 signal directly to the output - Applies to: CoaXPress
        TriggerCameraOutSelect_BypassGPI1,  //!< Bypass the GPI 1 signal directly to the output - Applies to: CoaXPress
        TriggerCameraOutSelect_BypassGPI2,  //!< Bypass the GPI 2 signal directly to the output - Applies to: CoaXPress
        TriggerCameraOutSelect_BypassGPI3,  //!< Bypass the GPI 3 signal directly to the output - Applies to: CoaXPress
        TriggerCameraOutSelect_BypassGPI4,  //!< Bypass the GPI 4 signal directly to the output - Applies to: CoaXPress
        TriggerCameraOutSelect_BypassGPI5,  //!< Bypass the GPI 5 signal directly to the output - Applies to: CoaXPress
        TriggerCameraOutSelect_BypassGPI6,  //!< Bypass the GPI 6 signal directly to the output - Applies to: CoaXPress
        TriggerCameraOutSelect_BypassGPI7,  //!< Bypass the GPI 7 signal directly to the output - Applies to: CoaXPress
        TriggerCameraOutSelect_CamAPulseGenerator0,  //!< The output source is set to CamAPulseGenerator0 - Applies to: CoaXPress
        TriggerCameraOutSelect_CamAPulseGenerator1,  //!< The output source is set to CamAPulseGenerator1 - Applies to: CoaXPress
        TriggerCameraOutSelect_CamAPulseGenerator2,  //!< The output source is set to CamAPulseGenerator2 - Applies to: CoaXPress
        TriggerCameraOutSelect_CamAPulseGenerator3,  //!< The output source is set to CamAPulseGenerator3 - Applies to: CoaXPress
        TriggerCameraOutSelect_CamBPulseGenerator0,  //!< The output source is set to Cam B Pulse Generator 0 - Applies to: CoaXPress
        TriggerCameraOutSelect_CamBPulseGenerator1,  //!< The output source is set to Cam B Pulse Generator 1 - Applies to: CoaXPress
        TriggerCameraOutSelect_CamBPulseGenerator2,  //!< The output source is set to Cam B Pulse Generator 2 - Applies to: CoaXPress
        TriggerCameraOutSelect_CamBPulseGenerator3,  //!< The output source is set to Cam B Pulse Generator 3 - Applies to: CoaXPress
        TriggerCameraOutSelect_CamCPulseGenerator0,  //!< The output source is set to Cam C Pulse Generator 0 - Applies to: CoaXPress
        TriggerCameraOutSelect_CamCPulseGenerator1,  //!< The output source is set to Cam C Pulse Generator 1 - Applies to: CoaXPress
        TriggerCameraOutSelect_CamCPulseGenerator2,  //!< The output source is set to Cam C Pulse Generator 2 - Applies to: CoaXPress
        TriggerCameraOutSelect_CamCPulseGenerator3,  //!< The output source is set to Cam C Pulse Generator 3 - Applies to: CoaXPress
        TriggerCameraOutSelect_CamDPulseGenerator0,  //!< The output source is set to Cam D Pulse Generator 0 - Applies to: CoaXPress
        TriggerCameraOutSelect_CamDPulseGenerator1,  //!< The output source is set to Cam D Pulse Generator 1 - Applies to: CoaXPress
        TriggerCameraOutSelect_CamDPulseGenerator2,  //!< The output source is set to Cam D Pulse Generator 2 - Applies to: CoaXPress
        TriggerCameraOutSelect_CamDPulseGenerator3,  //!< The output source is set to Cam D Pulse Generator 3 - Applies to: CoaXPress
        TriggerCameraOutSelect_GND,  //!< The output source is set to GND - Applies to: CoaXPress
        TriggerCameraOutSelect_NotBypassFronGPI0,  //!< The output source is set to NotBypassFrontGPI0 - Applies to: CoaXPress
        TriggerCameraOutSelect_NotBypassFronGPI1,  //!< The output source is set to NotBypassFrontGPI1 - Applies to: CoaXPress
        TriggerCameraOutSelect_NotBypassFronGPI2,  //!< The output source is set to NotBypassFrontGPI2 - Applies to: CoaXPress
        TriggerCameraOutSelect_NotBypassFronGPI3,  //!< The output source is set to NotBypassFrontGPI3 - Applies to: CoaXPress
        TriggerCameraOutSelect_NotBypassFrontGPI0,  //!< The output source is set to Not Bypass Front-GPI 0 - Applies to: CoaXPress
        TriggerCameraOutSelect_NotBypassFrontGPI1,  //!< The output source is set to Not Bypass Front-GPI 1 - Applies to: CoaXPress
        TriggerCameraOutSelect_NotBypassFrontGPI2,  //!< The output source is set to Not Bypass Front-GPI 2 - Applies to: CoaXPress
        TriggerCameraOutSelect_NotBypassFrontGPI3,  //!< The output source is set to Not Bypass Front-GPI 3 - Applies to: CoaXPress
        TriggerCameraOutSelect_NotBypassGPI0,  //!< Bypass the inverted GPI 0 signal directly to the output - Applies to: CoaXPress
        TriggerCameraOutSelect_NotBypassGPI1,  //!< Bypass the inverted GPI 1 signal directly to the output - Applies to: CoaXPress
        TriggerCameraOutSelect_NotBypassGPI2,  //!< Bypass the inverted GPI 2 signal directly to the output - Applies to: CoaXPress
        TriggerCameraOutSelect_NotBypassGPI3,  //!< Bypass the inverted GPI 3 signal directly to the output - Applies to: CoaXPress
        TriggerCameraOutSelect_NotBypassGPI4,  //!< Bypass the inverted GPI 4 signal directly to the output - Applies to: CoaXPress
        TriggerCameraOutSelect_NotBypassGPI5,  //!< Bypass the inverted GPI 5 signal directly to the output - Applies to: CoaXPress
        TriggerCameraOutSelect_NotBypassGPI6,  //!< Bypass the inverted GPI 6 signal directly to the output - Applies to: CoaXPress
        TriggerCameraOutSelect_NotBypassGPI7,  //!< Bypass the inverted GPI 7 signal directly to the output - Applies to: CoaXPress
        TriggerCameraOutSelect_NotCamAPulseGenerator0,  //!< The output source is set to NotCamAPulseGenerator0 - Applies to: CoaXPress
        TriggerCameraOutSelect_NotCamAPulseGenerator1,  //!< The output source is set to NotCamAPulseGenerator1 - Applies to: CoaXPress
        TriggerCameraOutSelect_NotCamAPulseGenerator2,  //!< The output source is set to NotCamAPulseGenerator2 - Applies to: CoaXPress
        TriggerCameraOutSelect_NotCamAPulseGenerator3,  //!< The output source is set to NotCamAPulseGenerator3 - Applies to: CoaXPress
        TriggerCameraOutSelect_NotCamBPulseGenerator0,  //!< The output source is set to Not Cam B Pulse Generator 0 - Applies to: CoaXPress
        TriggerCameraOutSelect_NotCamBPulseGenerator1,  //!< The output source is set to Not Cam B Pulse Generator 1 - Applies to: CoaXPress
        TriggerCameraOutSelect_NotCamBPulseGenerator2,  //!< The output source is set to Not Cam B Pulse Generator 2 - Applies to: CoaXPress
        TriggerCameraOutSelect_NotCamBPulseGenerator3,  //!< The output source is set to Not Cam B Pulse Generator 3 - Applies to: CoaXPress
        TriggerCameraOutSelect_NotCamCPulseGenerator0,  //!< The output source is set to Not Cam C Pulse Generator 0 - Applies to: CoaXPress
        TriggerCameraOutSelect_NotCamCPulseGenerator1,  //!< The output source is set to Not Cam C Pulse Generator 1 - Applies to: CoaXPress
        TriggerCameraOutSelect_NotCamCPulseGenerator2,  //!< The output source is set to Not Cam C Pulse Generator 2 - Applies to: CoaXPress
        TriggerCameraOutSelect_NotCamCPulseGenerator3,  //!< The output source is set to Not Cam C Pulse Generator 3 - Applies to: CoaXPress
        TriggerCameraOutSelect_NotCamDPulseGenerator0,  //!< The output source is set to Not Cam D Pulse Generator 0 - Applies to: CoaXPress
        TriggerCameraOutSelect_NotCamDPulseGenerator1,  //!< The output source is set to Not Cam D Pulse Generator 1 - Applies to: CoaXPress
        TriggerCameraOutSelect_NotCamDPulseGenerator2,  //!< The output source is set to Not Cam D Pulse Generator 2 - Applies to: CoaXPress
        TriggerCameraOutSelect_NotCamDPulseGenerator3,  //!< The output source is set to Not Cam D Pulse Generator 3 - Applies to: CoaXPress
        TriggerCameraOutSelect_NotPulseGenerator0,  //!< The output source is set to NotPulseGenerator0 - Applies to: CoaXPress
        TriggerCameraOutSelect_NotPulseGenerator1,  //!< The output source is set to NotPulseGenerator1 - Applies to: CoaXPress
        TriggerCameraOutSelect_NotPulseGenerator2,  //!< The output source is set to NotPulseGenerator2 - Applies to: CoaXPress
        TriggerCameraOutSelect_NotPulseGenerator3,  //!< The output source is set to NotPulseGenerator3 - Applies to: CoaXPress
        TriggerCameraOutSelect_PulseGenerator0,  //!< The output source is set to PulseGenerator0 - Applies to: CoaXPress
        TriggerCameraOutSelect_PulseGenerator1,  //!< The output source is set to PulseGenerator1 - Applies to: CoaXPress
        TriggerCameraOutSelect_PulseGenerator2,  //!< The output source is set to PulseGenerator2 - Applies to: CoaXPress
        TriggerCameraOutSelect_PulseGenerator3,  //!< The output source is set to PulseGenerator3 - Applies to: CoaXPress
        TriggerCameraOutSelect_VCC  //!< The output source is set to VCC - Applies to: CoaXPress
    };

    //! Valid values for TriggerExceededPeriodLimits
    enum TriggerExceededPeriodLimitsEnums
    {
        TriggerExceededPeriodLimits_No,  //!< The input signal frequency hasn't exceeded the maximum frequency defined by the Trigger Output Frequency parameter - Applies to: CoaXPress
        TriggerExceededPeriodLimits_Yes  //!< The input signal frequency has exceeded the maximum frequency defined by the Trigger Output Frequency parameter - Applies to: CoaXPress
    };

    //! Valid values for TriggerInPolarity
    enum TriggerInPolarityEnums
    {
        TriggerInPolarity_HighActive,  //!< The polarity is set to HighActive - Applies to: CoaXPress
        TriggerInPolarity_LowActive  //!< The polarity is set to LowActive - Applies to: CoaXPress
    };

    //! Valid values for TriggerInSource
    enum TriggerInSourceEnums
    {
        TriggerInSource_GPITriggerSource0,  //!< GPI Trigger Source 0 - Applies to: CoaXPress
        TriggerInSource_GPITriggerSource1,  //!< GPI Trigger Source 1 - Applies to: CoaXPress
        TriggerInSource_GPITriggerSource2,  //!< GPI Trigger Source 2 - Applies to: CoaXPress
        TriggerInSource_GPITriggerSource3,  //!< GPI Trigger Source 3 - Applies to: CoaXPress
        TriggerInSource_GPITriggerSource4,  //!< GPI Trigger Source 4 - Applies to: CoaXPress
        TriggerInSource_GPITriggerSource5,  //!< GPI Trigger Source 5 - Applies to: CoaXPress
        TriggerInSource_GPITriggerSource6,  //!< GPI Trigger Source 6 - Applies to: CoaXPress
        TriggerInSource_GPITriggerSource7,  //!< GPI Trigger Source 7 - Applies to: CoaXPress
        TriggerInSource_TriggerInSourceFrontGPI0,  //!< The trigger input source is set to TriggerInSourceFrontGPI0 - Applies to: CoaXPress
        TriggerInSource_TriggerInSourceFrontGPI1,  //!< The trigger input source is set to TriggerInSourceFrontGPI1 - Applies to: CoaXPress
        TriggerInSource_TriggerInSourceFrontGPI2,  //!< The trigger input source is set to TriggerInSourceFrontGPI2 - Applies to: CoaXPress
        TriggerInSource_TriggerInSourceFrontGPI3  //!< The trigger input source is set to TriggerInSourceFrontGPI3 - Applies to: CoaXPress
    };

    //! Valid values for TriggerInStatisticsPolarity
    enum TriggerInStatisticsPolarityEnums
    {
        TriggerInStatisticsPolarity_HighActive,  //!< The trigger polarity of the input specified by the Statistics Source parameter is set to High Active - Applies to: CoaXPress
        TriggerInStatisticsPolarity_LowActive  //!< The trigger polarity of the input specified by the Statistics Source parameter is set to Low Active - Applies to: CoaXPress
    };

    //! Valid values for TriggerInStatisticsSource
    enum TriggerInStatisticsSourceEnums
    {
        TriggerInStatisticsSource_GPITriggerSource0,  //!< GPI Trigger Source 0 - Applies to: CoaXPress
        TriggerInStatisticsSource_GPITriggerSource1,  //!< GPI Trigger Source 1 - Applies to: CoaXPress
        TriggerInStatisticsSource_GPITriggerSource2,  //!< GPI Trigger Source 2 - Applies to: CoaXPress
        TriggerInStatisticsSource_GPITriggerSource3,  //!< GPI Trigger Source 3 - Applies to: CoaXPress
        TriggerInStatisticsSource_GPITriggerSource4,  //!< GPI Trigger Source 4 - Applies to: CoaXPress
        TriggerInStatisticsSource_GPITriggerSource5,  //!< GPI Trigger Source 5 - Applies to: CoaXPress
        TriggerInStatisticsSource_GPITriggerSource6,  //!< GPI Trigger Source 6 - Applies to: CoaXPress
        TriggerInStatisticsSource_GPITriggerSource7,  //!< GPI Trigger Source 7 - Applies to: CoaXPress
        TriggerInStatisticsSource_TriggerInSourceFrontGPI0,  //!< The input source of the trigger statistics is set to Front GPI Trigger Source 0 - Applies to: CoaXPress
        TriggerInStatisticsSource_TriggerInSourceFrontGPI1,  //!< The input source of the trigger statistics is set to Front GPI Trigger Source 1 - Applies to: CoaXPress
        TriggerInStatisticsSource_TriggerInSourceFrontGPI2,  //!< The input source of the trigger statistics is set to Front GPI Trigger Source 2 - Applies to: CoaXPress
        TriggerInStatisticsSource_TriggerInSourceFrontGPI3  //!< The input source of the trigger statistics is set to Front GPI Trigger Source 3 - Applies to: CoaXPress
    };

    //! Valid values for TriggerOutSelectFrontGPO0
    enum TriggerOutSelectFrontGPO0Enums
    {
        TriggerOutSelectFrontGPO0_BypassFrontGPI0,  //!< The output source is set to BypassFrontGPI0 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_BypassFrontGPI1,  //!< The output source is set to BypassFrontGPI1 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_BypassFrontGPI2,  //!< The output source is set to BypassFrontGPI2 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_BypassFrontGPI3,  //!< The output source is set to BypassFrontGPI3 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_BypassGPI0,  //!< Bypass the GPI 0 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_BypassGPI1,  //!< Bypass the GPI 1 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_BypassGPI2,  //!< Bypass the GPI 2 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_BypassGPI3,  //!< Bypass the GPI 3 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_BypassGPI4,  //!< Bypass the GPI 4 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_BypassGPI5,  //!< Bypass the GPI 5 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_BypassGPI6,  //!< Bypass the GPI 6 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_BypassGPI7,  //!< Bypass the GPI 7 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_CamAPulseGenerator0,  //!< The output source is set to CamAPulseGenerator0 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_CamAPulseGenerator1,  //!< The output source is set to CamAPulseGenerator1 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_CamAPulseGenerator2,  //!< The output source is set to CamAPulseGenerator2 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_CamAPulseGenerator3,  //!< The output source is set to CamAPulseGenerator3 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_CamBPulseGenerator0,  //!< The output source is set to Cam B Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_CamBPulseGenerator1,  //!< The output source is set to Cam B Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_CamBPulseGenerator2,  //!< The output source is set to Cam B Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_CamBPulseGenerator3,  //!< The output source is set to Cam B Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_CamCPulseGenerator0,  //!< The output source is set to Cam C Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_CamCPulseGenerator1,  //!< The output source is set to Cam C Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_CamCPulseGenerator2,  //!< The output source is set to Cam C Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_CamCPulseGenerator3,  //!< The output source is set to Cam C Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_CamDPulseGenerator0,  //!< The output source is set to Cam D Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_CamDPulseGenerator1,  //!< The output source is set to Cam D Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_CamDPulseGenerator2,  //!< The output source is set to Cam D Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_CamDPulseGenerator3,  //!< The output source is set to Cam D Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_GND,  //!< The output source is set to GND - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_NotBypassFronGPI0,  //!< The output source is set to NotBypassFrontGPI0 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_NotBypassFronGPI1,  //!< The output source is set to NotBypassFrontGPI1 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_NotBypassFronGPI2,  //!< The output source is set to NotBypassFrontGPI2 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_NotBypassFronGPI3,  //!< The output source is set to NotBypassFrontGPI3 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_NotBypassFrontGPI0,  //!< The output source is set to Not Bypass Front-GPI 0 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_NotBypassFrontGPI1,  //!< The output source is set to Not Bypass Front-GPI 1 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_NotBypassFrontGPI2,  //!< The output source is set to Not Bypass Front-GPI 2 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_NotBypassFrontGPI3,  //!< The output source is set to Not Bypass Front-GPI 3 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_NotBypassGPI0,  //!< Bypass the inverted GPI 0 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_NotBypassGPI1,  //!< Bypass the inverted GPI 1 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_NotBypassGPI2,  //!< Bypass the inverted GPI 2 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_NotBypassGPI3,  //!< Bypass the inverted GPI 3 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_NotBypassGPI4,  //!< Bypass the inverted GPI 4 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_NotBypassGPI5,  //!< Bypass the inverted GPI 5 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_NotBypassGPI6,  //!< Bypass the inverted GPI 6 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_NotBypassGPI7,  //!< Bypass the inverted GPI 7 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_NotCamAPulseGenerator0,  //!< The output source is set to NotCamAPulseGenerator0 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_NotCamAPulseGenerator1,  //!< The output source is set to NotCamAPulseGenerator1 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_NotCamAPulseGenerator2,  //!< The output source is set to NotCamAPulseGenerator2 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_NotCamAPulseGenerator3,  //!< The output source is set to NotCamAPulseGenerator3 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_NotCamBPulseGenerator0,  //!< The output source is set to Not Cam B Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_NotCamBPulseGenerator1,  //!< The output source is set to Not Cam B Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_NotCamBPulseGenerator2,  //!< The output source is set to Not Cam B Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_NotCamBPulseGenerator3,  //!< The output source is set to Not Cam B Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_NotCamCPulseGenerator0,  //!< The output source is set to Not Cam C Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_NotCamCPulseGenerator1,  //!< The output source is set to Not Cam C Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_NotCamCPulseGenerator2,  //!< The output source is set to Not Cam C Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_NotCamCPulseGenerator3,  //!< The output source is set to Not Cam C Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_NotCamDPulseGenerator0,  //!< The output source is set to Not Cam D Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_NotCamDPulseGenerator1,  //!< The output source is set to Not Cam D Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_NotCamDPulseGenerator2,  //!< The output source is set to Not Cam D Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_NotCamDPulseGenerator3,  //!< The output source is set to Not Cam D Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_NotPulseGenerator0,  //!< The output source is set to NotPulseGenerator0 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_NotPulseGenerator1,  //!< The output source is set to NotPulseGenerator1 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_NotPulseGenerator2,  //!< The output source is set to NotPulseGenerator2 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_NotPulseGenerator3,  //!< The output source is set to NotPulseGenerator3 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_PulseGenerator0,  //!< The output source is set to PulseGenerator0 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_PulseGenerator1,  //!< The output source is set to PulseGenerator1 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_PulseGenerator2,  //!< The output source is set to PulseGenerator2 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_PulseGenerator3,  //!< The output source is set to PulseGenerator3 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO0_VCC  //!< The output source is set to VCC - Applies to: CoaXPress
    };

    //! Valid values for TriggerOutSelectFrontGPO1
    enum TriggerOutSelectFrontGPO1Enums
    {
        TriggerOutSelectFrontGPO1_BypassFrontGPI0,  //!< The output source is set to BypassFrontGPI0 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_BypassFrontGPI1,  //!< The output source is set to BypassFrontGPI1 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_BypassFrontGPI2,  //!< The output source is set to BypassFrontGPI2 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_BypassFrontGPI3,  //!< The output source is set to BypassFrontGPI3 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_BypassGPI0,  //!< Bypass the GPI 0 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_BypassGPI1,  //!< Bypass the GPI 1 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_BypassGPI2,  //!< Bypass the GPI 2 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_BypassGPI3,  //!< Bypass the GPI 3 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_BypassGPI4,  //!< Bypass the GPI 4 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_BypassGPI5,  //!< Bypass the GPI 5 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_BypassGPI6,  //!< Bypass the GPI 6 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_BypassGPI7,  //!< Bypass the GPI 7 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_CamAPulseGenerator0,  //!< The output source is set to CamAPulseGenerator0 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_CamAPulseGenerator1,  //!< The output source is set to CamAPulseGenerator1 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_CamAPulseGenerator2,  //!< The output source is set to CamAPulseGenerator2 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_CamAPulseGenerator3,  //!< The output source is set to CamAPulseGenerator3 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_CamBPulseGenerator0,  //!< The output source is set to Cam B Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_CamBPulseGenerator1,  //!< The output source is set to Cam B Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_CamBPulseGenerator2,  //!< The output source is set to Cam B Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_CamBPulseGenerator3,  //!< The output source is set to Cam B Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_CamCPulseGenerator0,  //!< The output source is set to Cam C Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_CamCPulseGenerator1,  //!< The output source is set to Cam C Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_CamCPulseGenerator2,  //!< The output source is set to Cam C Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_CamCPulseGenerator3,  //!< The output source is set to Cam C Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_CamDPulseGenerator0,  //!< The output source is set to Cam D Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_CamDPulseGenerator1,  //!< The output source is set to Cam D Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_CamDPulseGenerator2,  //!< The output source is set to Cam D Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_CamDPulseGenerator3,  //!< The output source is set to Cam D Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_GND,  //!< The output source is set to GND - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_NotBypassFronGPI0,  //!< The output source is set to NotBypassFrontGPI0 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_NotBypassFronGPI1,  //!< The output source is set to NotBypassFrontGPI1 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_NotBypassFronGPI2,  //!< The output source is set to NotBypassFrontGPI2 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_NotBypassFronGPI3,  //!< The output source is set to NotBypassFrontGPI3 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_NotBypassFrontGPI0,  //!< The output source is set to Not Bypass Front-GPI 0 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_NotBypassFrontGPI1,  //!< The output source is set to Not Bypass Front-GPI 1 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_NotBypassFrontGPI2,  //!< The output source is set to Not Bypass Front-GPI 2 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_NotBypassFrontGPI3,  //!< The output source is set to Not Bypass Front-GPI 3 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_NotBypassGPI0,  //!< Bypass the inverted GPI 0 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_NotBypassGPI1,  //!< Bypass the inverted GPI 1 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_NotBypassGPI2,  //!< Bypass the inverted GPI 2 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_NotBypassGPI3,  //!< Bypass the inverted GPI 3 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_NotBypassGPI4,  //!< Bypass the inverted GPI 4 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_NotBypassGPI5,  //!< Bypass the inverted GPI 5 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_NotBypassGPI6,  //!< Bypass the inverted GPI 6 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_NotBypassGPI7,  //!< Bypass the inverted GPI 7 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_NotCamAPulseGenerator0,  //!< The output source is set to NotCamAPulseGenerator0 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_NotCamAPulseGenerator1,  //!< The output source is set to NotCamAPulseGenerator1 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_NotCamAPulseGenerator2,  //!< The output source is set to NotCamAPulseGenerator2 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_NotCamAPulseGenerator3,  //!< The output source is set to NotCamAPulseGenerator3 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_NotCamBPulseGenerator0,  //!< The output source is set to Not Cam B Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_NotCamBPulseGenerator1,  //!< The output source is set to Not Cam B Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_NotCamBPulseGenerator2,  //!< The output source is set to Not Cam B Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_NotCamBPulseGenerator3,  //!< The output source is set to Not Cam B Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_NotCamCPulseGenerator0,  //!< The output source is set to Not Cam C Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_NotCamCPulseGenerator1,  //!< The output source is set to Not Cam C Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_NotCamCPulseGenerator2,  //!< The output source is set to Not Cam C Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_NotCamCPulseGenerator3,  //!< The output source is set to Not Cam C Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_NotCamDPulseGenerator0,  //!< The output source is set to Not Cam D Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_NotCamDPulseGenerator1,  //!< The output source is set to Not Cam D Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_NotCamDPulseGenerator2,  //!< The output source is set to Not Cam D Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_NotCamDPulseGenerator3,  //!< The output source is set to Not Cam D Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_NotPulseGenerator0,  //!< The output source is set to NotPulseGenerator0 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_NotPulseGenerator1,  //!< The output source is set to NotPulseGenerator1 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_NotPulseGenerator2,  //!< The output source is set to NotPulseGenerator2 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_NotPulseGenerator3,  //!< The output source is set to NotPulseGenerator3 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_PulseGenerator0,  //!< The output source is set to PulseGenerator0 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_PulseGenerator1,  //!< The output source is set to PulseGenerator1 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_PulseGenerator2,  //!< The output source is set to PulseGenerator2 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_PulseGenerator3,  //!< The output source is set to PulseGenerator3 - Applies to: CoaXPress
        TriggerOutSelectFrontGPO1_VCC  //!< The output source is set to VCC - Applies to: CoaXPress
    };

    //! Valid values for TriggerOutSelectGPO0
    enum TriggerOutSelectGPO0Enums
    {
        TriggerOutSelectGPO0_BypassFrontGPI0,  //!< Bypass the Front GPI 0 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO0_BypassFrontGPI1,  //!< Bypass the Front GPI 1 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO0_BypassFrontGPI2,  //!< Bypass the Front GPI 2 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO0_BypassFrontGPI3,  //!< Bypass the Front GPI 3 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO0_BypassGPI0,  //!< Bypass the GPI 0 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO0_BypassGPI1,  //!< Bypass the GPI 1 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO0_BypassGPI2,  //!< Bypass the GPI 2 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO0_BypassGPI3,  //!< Bypass the GPI 3 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO0_BypassGPI4,  //!< Bypass the GPI 4 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO0_BypassGPI5,  //!< Bypass the GPI 5 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO0_BypassGPI6,  //!< Bypass the GPI 6 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO0_BypassGPI7,  //!< Bypass the GPI 7 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO0_CamAPulseGenerator0,  //!< Camera port A Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO0_CamAPulseGenerator1,  //!< Camera port A Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO0_CamAPulseGenerator2,  //!< Camera port A Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO0_CamAPulseGenerator3,  //!< Camera port A Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO0_CamBPulseGenerator0,  //!< The output source is set to Cam B Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO0_CamBPulseGenerator1,  //!< The output source is set to Cam B Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO0_CamBPulseGenerator2,  //!< The output source is set to Cam B Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO0_CamBPulseGenerator3,  //!< The output source is set to Cam B Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO0_CamCPulseGenerator0,  //!< The output source is set to Cam C Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO0_CamCPulseGenerator1,  //!< The output source is set to Cam C Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO0_CamCPulseGenerator2,  //!< The output source is set to Cam C Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO0_CamCPulseGenerator3,  //!< The output source is set to Cam C Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO0_CamDPulseGenerator0,  //!< The output source is set to Cam D Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO0_CamDPulseGenerator1,  //!< The output source is set to Cam D Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO0_CamDPulseGenerator2,  //!< The output source is set to Cam D Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO0_CamDPulseGenerator3,  //!< The output source is set to Cam D Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO0_GND,  //!< A static zero - Applies to: CoaXPress
        TriggerOutSelectGPO0_NotBypassFrontGPI0,  //!< The output source is set to Not Bypass Front-GPI 0 - Applies to: CoaXPress
        TriggerOutSelectGPO0_NotBypassFrontGPI1,  //!< The output source is set to Not Bypass Front-GPI 1 - Applies to: CoaXPress
        TriggerOutSelectGPO0_NotBypassFrontGPI2,  //!< The output source is set to Not Bypass Front-GPI 2 - Applies to: CoaXPress
        TriggerOutSelectGPO0_NotBypassFrontGPI3,  //!< The output source is set to Not Bypass Front-GPI 3 - Applies to: CoaXPress
        TriggerOutSelectGPO0_NotBypassGPI0,  //!< Bypass the inverted GPI 0 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO0_NotBypassGPI1,  //!< Bypass the inverted GPI 1 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO0_NotBypassGPI2,  //!< Bypass the inverted GPI 2 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO0_NotBypassGPI3,  //!< Bypass the inverted GPI 3 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO0_NotBypassGPI4,  //!< Bypass the inverted GPI 4 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO0_NotBypassGPI5,  //!< Bypass the inverted GPI 5 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO0_NotBypassGPI6,  //!< Bypass the inverted GPI 6 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO0_NotBypassGPI7,  //!< Bypass the inverted GPI 7 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO0_NotCamAPulseGenerator0,  //!< Inverted signal of camera port A Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO0_NotCamAPulseGenerator1,  //!< Inverted signal of camera port A Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO0_NotCamAPulseGenerator2,  //!< Inverted signal of camera port A Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO0_NotCamAPulseGenerator3,  //!< Inverted signal of camera port A Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO0_NotCamBPulseGenerator0,  //!< The output source is set to Not Cam B Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO0_NotCamBPulseGenerator1,  //!< The output source is set to Not Cam B Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO0_NotCamBPulseGenerator2,  //!< The output source is set to Not Cam B Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO0_NotCamBPulseGenerator3,  //!< The output source is set to Not Cam B Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO0_NotCamCPulseGenerator0,  //!< The output source is set to Not Cam C Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO0_NotCamCPulseGenerator1,  //!< The output source is set to Not Cam C Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO0_NotCamCPulseGenerator2,  //!< The output source is set to Not Cam C Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO0_NotCamCPulseGenerator3,  //!< The output source is set to Not Cam C Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO0_NotCamDPulseGenerator0,  //!< The output source is set to Not Cam D Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO0_NotCamDPulseGenerator1,  //!< The output source is set to Not Cam D Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO0_NotCamDPulseGenerator2,  //!< The output source is set to Not Cam D Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO0_NotCamDPulseGenerator3,  //!< The output source is set to Not Cam D Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO0_NotPulseGenerator0,  //!< Inverted signal of pulse generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO0_NotPulseGenerator1,  //!< Inverted signal of pulse generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO0_NotPulseGenerator2,  //!< Inverted signal of pulse generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO0_NotPulseGenerator3,  //!< Inverted signal of pulse generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO0_PulseGenerator0,  //!< The source for the output statistics is set to Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO0_PulseGenerator1,  //!< The source for the output statistics is set to Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO0_PulseGenerator2,  //!< The source for the output statistics is set to Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO0_PulseGenerator3,  //!< The source for the output statistics is set to Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO0_VCC  //!< A static one - Applies to: CoaXPress
    };

    //! Valid values for TriggerOutSelectGPO1
    enum TriggerOutSelectGPO1Enums
    {
        TriggerOutSelectGPO1_BypassFrontGPI0,  //!< Bypass the Front GPI 0 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO1_BypassFrontGPI1,  //!< Bypass the Front GPI 1 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO1_BypassFrontGPI2,  //!< Bypass the Front GPI 2 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO1_BypassFrontGPI3,  //!< Bypass the Front GPI 3 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO1_BypassGPI0,  //!< Bypass the GPI 0 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO1_BypassGPI1,  //!< Bypass the GPI 1 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO1_BypassGPI2,  //!< Bypass the GPI 2 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO1_BypassGPI3,  //!< Bypass the GPI 3 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO1_BypassGPI4,  //!< Bypass the GPI 4 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO1_BypassGPI5,  //!< Bypass the GPI 5 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO1_BypassGPI6,  //!< Bypass the GPI 6 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO1_BypassGPI7,  //!< Bypass the GPI 7 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO1_CamAPulseGenerator0,  //!< Camera port A Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO1_CamAPulseGenerator1,  //!< Camera port A Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO1_CamAPulseGenerator2,  //!< Camera port A Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO1_CamAPulseGenerator3,  //!< Camera port A Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO1_CamBPulseGenerator0,  //!< The output source is set to Cam B Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO1_CamBPulseGenerator1,  //!< The output source is set to Cam B Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO1_CamBPulseGenerator2,  //!< The output source is set to Cam B Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO1_CamBPulseGenerator3,  //!< The output source is set to Cam B Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO1_CamCPulseGenerator0,  //!< The output source is set to Cam C Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO1_CamCPulseGenerator1,  //!< The output source is set to Cam C Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO1_CamCPulseGenerator2,  //!< The output source is set to Cam C Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO1_CamCPulseGenerator3,  //!< The output source is set to Cam C Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO1_CamDPulseGenerator0,  //!< The output source is set to Cam D Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO1_CamDPulseGenerator1,  //!< The output source is set to Cam D Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO1_CamDPulseGenerator2,  //!< The output source is set to Cam D Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO1_CamDPulseGenerator3,  //!< The output source is set to Cam D Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO1_GND,  //!< A static zero - Applies to: CoaXPress
        TriggerOutSelectGPO1_NotBypassFrontGPI0,  //!< The output source is set to Not Bypass Front-GPI 0 - Applies to: CoaXPress
        TriggerOutSelectGPO1_NotBypassFrontGPI1,  //!< The output source is set to Not Bypass Front-GPI 1 - Applies to: CoaXPress
        TriggerOutSelectGPO1_NotBypassFrontGPI2,  //!< The output source is set to Not Bypass Front-GPI 2 - Applies to: CoaXPress
        TriggerOutSelectGPO1_NotBypassFrontGPI3,  //!< The output source is set to Not Bypass Front-GPI 3 - Applies to: CoaXPress
        TriggerOutSelectGPO1_NotBypassGPI0,  //!< Bypass the inverted GPI 0 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO1_NotBypassGPI1,  //!< Bypass the inverted GPI 1 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO1_NotBypassGPI2,  //!< Bypass the inverted GPI 2 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO1_NotBypassGPI3,  //!< Bypass the inverted GPI 3 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO1_NotBypassGPI4,  //!< Bypass the inverted GPI 4 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO1_NotBypassGPI5,  //!< Bypass the inverted GPI 5 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO1_NotBypassGPI6,  //!< Bypass the inverted GPI 6 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO1_NotBypassGPI7,  //!< Bypass the inverted GPI 7 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO1_NotCamAPulseGenerator0,  //!< Inverted signal of camera port A Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO1_NotCamAPulseGenerator1,  //!< Inverted signal of camera port A Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO1_NotCamAPulseGenerator2,  //!< Inverted signal of camera port A Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO1_NotCamAPulseGenerator3,  //!< Inverted signal of camera port A Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO1_NotCamBPulseGenerator0,  //!< The output source is set to Not Cam B Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO1_NotCamBPulseGenerator1,  //!< The output source is set to Not Cam B Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO1_NotCamBPulseGenerator2,  //!< The output source is set to Not Cam B Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO1_NotCamBPulseGenerator3,  //!< The output source is set to Not Cam B Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO1_NotCamCPulseGenerator0,  //!< The output source is set to Not Cam C Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO1_NotCamCPulseGenerator1,  //!< The output source is set to Not Cam C Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO1_NotCamCPulseGenerator2,  //!< The output source is set to Not Cam C Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO1_NotCamCPulseGenerator3,  //!< The output source is set to Not Cam C Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO1_NotCamDPulseGenerator0,  //!< The output source is set to Not Cam D Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO1_NotCamDPulseGenerator1,  //!< The output source is set to Not Cam D Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO1_NotCamDPulseGenerator2,  //!< The output source is set to Not Cam D Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO1_NotCamDPulseGenerator3,  //!< The output source is set to Not Cam D Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO1_NotPulseGenerator0,  //!< Inverted signal of pulse generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO1_NotPulseGenerator1,  //!< Inverted signal of pulse generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO1_NotPulseGenerator2,  //!< Inverted signal of pulse generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO1_NotPulseGenerator3,  //!< Inverted signal of pulse generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO1_PulseGenerator0,  //!< The source for the output statistics is set to Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO1_PulseGenerator1,  //!< The source for the output statistics is set to Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO1_PulseGenerator2,  //!< The source for the output statistics is set to Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO1_PulseGenerator3,  //!< The source for the output statistics is set to Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO1_VCC  //!< A static one - Applies to: CoaXPress
    };

    //! Valid values for TriggerOutSelectGPO2
    enum TriggerOutSelectGPO2Enums
    {
        TriggerOutSelectGPO2_BypassFrontGPI0,  //!< Bypass the Front GPI 0 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO2_BypassFrontGPI1,  //!< Bypass the Front GPI 1 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO2_BypassFrontGPI2,  //!< Bypass the Front GPI 2 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO2_BypassFrontGPI3,  //!< Bypass the Front GPI 3 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO2_BypassGPI0,  //!< Bypass the GPI 0 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO2_BypassGPI1,  //!< Bypass the GPI 1 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO2_BypassGPI2,  //!< Bypass the GPI 2 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO2_BypassGPI3,  //!< Bypass the GPI 3 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO2_BypassGPI4,  //!< Bypass the GPI 4 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO2_BypassGPI5,  //!< Bypass the GPI 5 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO2_BypassGPI6,  //!< Bypass the GPI 6 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO2_BypassGPI7,  //!< Bypass the GPI 7 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO2_CamAPulseGenerator0,  //!< Camera port A Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO2_CamAPulseGenerator1,  //!< Camera port A Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO2_CamAPulseGenerator2,  //!< Camera port A Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO2_CamAPulseGenerator3,  //!< Camera port A Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO2_CamBPulseGenerator0,  //!< The output source is set to Cam B Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO2_CamBPulseGenerator1,  //!< The output source is set to Cam B Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO2_CamBPulseGenerator2,  //!< The output source is set to Cam B Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO2_CamBPulseGenerator3,  //!< The output source is set to Cam B Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO2_CamCPulseGenerator0,  //!< The output source is set to Cam C Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO2_CamCPulseGenerator1,  //!< The output source is set to Cam C Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO2_CamCPulseGenerator2,  //!< The output source is set to Cam C Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO2_CamCPulseGenerator3,  //!< The output source is set to Cam C Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO2_CamDPulseGenerator0,  //!< The output source is set to Cam D Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO2_CamDPulseGenerator1,  //!< The output source is set to Cam D Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO2_CamDPulseGenerator2,  //!< The output source is set to Cam D Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO2_CamDPulseGenerator3,  //!< The output source is set to Cam D Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO2_GND,  //!< A static zero - Applies to: CoaXPress
        TriggerOutSelectGPO2_NotBypassFrontGPI0,  //!< The output source is set to Not Bypass Front-GPI 0 - Applies to: CoaXPress
        TriggerOutSelectGPO2_NotBypassFrontGPI1,  //!< The output source is set to Not Bypass Front-GPI 1 - Applies to: CoaXPress
        TriggerOutSelectGPO2_NotBypassFrontGPI2,  //!< The output source is set to Not Bypass Front-GPI 2 - Applies to: CoaXPress
        TriggerOutSelectGPO2_NotBypassFrontGPI3,  //!< The output source is set to Not Bypass Front-GPI 3 - Applies to: CoaXPress
        TriggerOutSelectGPO2_NotBypassGPI0,  //!< Bypass the inverted GPI 0 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO2_NotBypassGPI1,  //!< Bypass the inverted GPI 1 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO2_NotBypassGPI2,  //!< Bypass the inverted GPI 2 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO2_NotBypassGPI3,  //!< Bypass the inverted GPI 3 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO2_NotBypassGPI4,  //!< Bypass the inverted GPI 4 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO2_NotBypassGPI5,  //!< Bypass the inverted GPI 5 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO2_NotBypassGPI6,  //!< Bypass the inverted GPI 6 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO2_NotBypassGPI7,  //!< Bypass the inverted GPI 7 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO2_NotCamAPulseGenerator0,  //!< Inverted signal of camera port A Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO2_NotCamAPulseGenerator1,  //!< Inverted signal of camera port A Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO2_NotCamAPulseGenerator2,  //!< Inverted signal of camera port A Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO2_NotCamAPulseGenerator3,  //!< Inverted signal of camera port A Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO2_NotCamBPulseGenerator0,  //!< The output source is set to Not Cam B Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO2_NotCamBPulseGenerator1,  //!< The output source is set to Not Cam B Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO2_NotCamBPulseGenerator2,  //!< The output source is set to Not Cam B Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO2_NotCamBPulseGenerator3,  //!< The output source is set to Not Cam B Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO2_NotCamCPulseGenerator0,  //!< The output source is set to Not Cam C Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO2_NotCamCPulseGenerator1,  //!< The output source is set to Not Cam C Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO2_NotCamCPulseGenerator2,  //!< The output source is set to Not Cam C Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO2_NotCamCPulseGenerator3,  //!< The output source is set to Not Cam C Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO2_NotCamDPulseGenerator0,  //!< The output source is set to Not Cam D Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO2_NotCamDPulseGenerator1,  //!< The output source is set to Not Cam D Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO2_NotCamDPulseGenerator2,  //!< The output source is set to Not Cam D Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO2_NotCamDPulseGenerator3,  //!< The output source is set to Not Cam D Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO2_NotPulseGenerator0,  //!< Inverted signal of pulse generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO2_NotPulseGenerator1,  //!< Inverted signal of pulse generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO2_NotPulseGenerator2,  //!< Inverted signal of pulse generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO2_NotPulseGenerator3,  //!< Inverted signal of pulse generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO2_PulseGenerator0,  //!< The source for the output statistics is set to Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO2_PulseGenerator1,  //!< The source for the output statistics is set to Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO2_PulseGenerator2,  //!< The source for the output statistics is set to Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO2_PulseGenerator3,  //!< The source for the output statistics is set to Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO2_VCC  //!< A static one - Applies to: CoaXPress
    };

    //! Valid values for TriggerOutSelectGPO3
    enum TriggerOutSelectGPO3Enums
    {
        TriggerOutSelectGPO3_BypassFrontGPI0,  //!< Bypass the Front GPI 0 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO3_BypassFrontGPI1,  //!< Bypass the Front GPI 1 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO3_BypassFrontGPI2,  //!< Bypass the Front GPI 2 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO3_BypassFrontGPI3,  //!< Bypass the Front GPI 3 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO3_BypassGPI0,  //!< Bypass the GPI 0 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO3_BypassGPI1,  //!< Bypass the GPI 1 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO3_BypassGPI2,  //!< Bypass the GPI 2 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO3_BypassGPI3,  //!< Bypass the GPI 3 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO3_BypassGPI4,  //!< Bypass the GPI 4 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO3_BypassGPI5,  //!< Bypass the GPI 5 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO3_BypassGPI6,  //!< Bypass the GPI 6 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO3_BypassGPI7,  //!< Bypass the GPI 7 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO3_CamAPulseGenerator0,  //!< Camera port A Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO3_CamAPulseGenerator1,  //!< Camera port A Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO3_CamAPulseGenerator2,  //!< Camera port A Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO3_CamAPulseGenerator3,  //!< Camera port A Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO3_CamBPulseGenerator0,  //!< The output source is set to Cam B Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO3_CamBPulseGenerator1,  //!< The output source is set to Cam B Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO3_CamBPulseGenerator2,  //!< The output source is set to Cam B Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO3_CamBPulseGenerator3,  //!< The output source is set to Cam B Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO3_CamCPulseGenerator0,  //!< The output source is set to Cam C Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO3_CamCPulseGenerator1,  //!< The output source is set to Cam C Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO3_CamCPulseGenerator2,  //!< The output source is set to Cam C Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO3_CamCPulseGenerator3,  //!< The output source is set to Cam C Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO3_CamDPulseGenerator0,  //!< The output source is set to Cam D Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO3_CamDPulseGenerator1,  //!< The output source is set to Cam D Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO3_CamDPulseGenerator2,  //!< The output source is set to Cam D Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO3_CamDPulseGenerator3,  //!< The output source is set to Cam D Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO3_GND,  //!< A static zero - Applies to: CoaXPress
        TriggerOutSelectGPO3_NotBypassFrontGPI0,  //!< The output source is set to Not Bypass Front-GPI 0 - Applies to: CoaXPress
        TriggerOutSelectGPO3_NotBypassFrontGPI1,  //!< The output source is set to Not Bypass Front-GPI 1 - Applies to: CoaXPress
        TriggerOutSelectGPO3_NotBypassFrontGPI2,  //!< The output source is set to Not Bypass Front-GPI 2 - Applies to: CoaXPress
        TriggerOutSelectGPO3_NotBypassFrontGPI3,  //!< The output source is set to Not Bypass Front-GPI 3 - Applies to: CoaXPress
        TriggerOutSelectGPO3_NotBypassGPI0,  //!< Bypass the inverted GPI 0 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO3_NotBypassGPI1,  //!< Bypass the inverted GPI 1 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO3_NotBypassGPI2,  //!< Bypass the inverted GPI 2 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO3_NotBypassGPI3,  //!< Bypass the inverted GPI 3 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO3_NotBypassGPI4,  //!< Bypass the inverted GPI 4 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO3_NotBypassGPI5,  //!< Bypass the inverted GPI 5 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO3_NotBypassGPI6,  //!< Bypass the inverted GPI 6 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO3_NotBypassGPI7,  //!< Bypass the inverted GPI 7 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO3_NotCamAPulseGenerator0,  //!< Inverted signal of camera port A Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO3_NotCamAPulseGenerator1,  //!< Inverted signal of camera port A Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO3_NotCamAPulseGenerator2,  //!< Inverted signal of camera port A Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO3_NotCamAPulseGenerator3,  //!< Inverted signal of camera port A Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO3_NotCamBPulseGenerator0,  //!< The output source is set to Not Cam B Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO3_NotCamBPulseGenerator1,  //!< The output source is set to Not Cam B Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO3_NotCamBPulseGenerator2,  //!< The output source is set to Not Cam B Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO3_NotCamBPulseGenerator3,  //!< The output source is set to Not Cam B Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO3_NotCamCPulseGenerator0,  //!< The output source is set to Not Cam C Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO3_NotCamCPulseGenerator1,  //!< The output source is set to Not Cam C Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO3_NotCamCPulseGenerator2,  //!< The output source is set to Not Cam C Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO3_NotCamCPulseGenerator3,  //!< The output source is set to Not Cam C Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO3_NotCamDPulseGenerator0,  //!< The output source is set to Not Cam D Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO3_NotCamDPulseGenerator1,  //!< The output source is set to Not Cam D Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO3_NotCamDPulseGenerator2,  //!< The output source is set to Not Cam D Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO3_NotCamDPulseGenerator3,  //!< The output source is set to Not Cam D Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO3_NotPulseGenerator0,  //!< Inverted signal of pulse generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO3_NotPulseGenerator1,  //!< Inverted signal of pulse generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO3_NotPulseGenerator2,  //!< Inverted signal of pulse generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO3_NotPulseGenerator3,  //!< Inverted signal of pulse generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO3_PulseGenerator0,  //!< The source for the output statistics is set to Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO3_PulseGenerator1,  //!< The source for the output statistics is set to Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO3_PulseGenerator2,  //!< The source for the output statistics is set to Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO3_PulseGenerator3,  //!< The source for the output statistics is set to Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO3_VCC  //!< A static one - Applies to: CoaXPress
    };

    //! Valid values for TriggerOutSelectGPO4
    enum TriggerOutSelectGPO4Enums
    {
        TriggerOutSelectGPO4_BypassFrontGPI0,  //!< Bypass the Front GPI 0 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO4_BypassFrontGPI1,  //!< Bypass the Front GPI 1 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO4_BypassFrontGPI2,  //!< Bypass the Front GPI 2 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO4_BypassFrontGPI3,  //!< Bypass the Front GPI 3 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO4_BypassGPI0,  //!< Bypass the GPI 0 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO4_BypassGPI1,  //!< Bypass the GPI 1 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO4_BypassGPI2,  //!< Bypass the GPI 2 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO4_BypassGPI3,  //!< Bypass the GPI 3 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO4_BypassGPI4,  //!< Bypass the GPI 4 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO4_BypassGPI5,  //!< Bypass the GPI 5 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO4_BypassGPI6,  //!< Bypass the GPI 6 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO4_BypassGPI7,  //!< Bypass the GPI 7 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO4_CamAPulseGenerator0,  //!< Camera port A Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO4_CamAPulseGenerator1,  //!< Camera port A Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO4_CamAPulseGenerator2,  //!< Camera port A Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO4_CamAPulseGenerator3,  //!< Camera port A Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO4_CamBPulseGenerator0,  //!< The output source is set to Cam B Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO4_CamBPulseGenerator1,  //!< The output source is set to Cam B Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO4_CamBPulseGenerator2,  //!< The output source is set to Cam B Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO4_CamBPulseGenerator3,  //!< The output source is set to Cam B Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO4_CamCPulseGenerator0,  //!< The output source is set to Cam C Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO4_CamCPulseGenerator1,  //!< The output source is set to Cam C Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO4_CamCPulseGenerator2,  //!< The output source is set to Cam C Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO4_CamCPulseGenerator3,  //!< The output source is set to Cam C Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO4_CamDPulseGenerator0,  //!< The output source is set to Cam D Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO4_CamDPulseGenerator1,  //!< The output source is set to Cam D Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO4_CamDPulseGenerator2,  //!< The output source is set to Cam D Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO4_CamDPulseGenerator3,  //!< The output source is set to Cam D Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO4_GND,  //!< A static zero - Applies to: CoaXPress
        TriggerOutSelectGPO4_NotBypassFrontGPI0,  //!< The output source is set to Not Bypass Front-GPI 0 - Applies to: CoaXPress
        TriggerOutSelectGPO4_NotBypassFrontGPI1,  //!< The output source is set to Not Bypass Front-GPI 1 - Applies to: CoaXPress
        TriggerOutSelectGPO4_NotBypassFrontGPI2,  //!< The output source is set to Not Bypass Front-GPI 2 - Applies to: CoaXPress
        TriggerOutSelectGPO4_NotBypassFrontGPI3,  //!< The output source is set to Not Bypass Front-GPI 3 - Applies to: CoaXPress
        TriggerOutSelectGPO4_NotBypassGPI0,  //!< Bypass the inverted GPI 0 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO4_NotBypassGPI1,  //!< Bypass the inverted GPI 1 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO4_NotBypassGPI2,  //!< Bypass the inverted GPI 2 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO4_NotBypassGPI3,  //!< Bypass the inverted GPI 3 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO4_NotBypassGPI4,  //!< Bypass the inverted GPI 4 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO4_NotBypassGPI5,  //!< Bypass the inverted GPI 5 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO4_NotBypassGPI6,  //!< Bypass the inverted GPI 6 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO4_NotBypassGPI7,  //!< Bypass the inverted GPI 7 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO4_NotCamAPulseGenerator0,  //!< Inverted signal of camera port A Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO4_NotCamAPulseGenerator1,  //!< Inverted signal of camera port A Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO4_NotCamAPulseGenerator2,  //!< Inverted signal of camera port A Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO4_NotCamAPulseGenerator3,  //!< Inverted signal of camera port A Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO4_NotCamBPulseGenerator0,  //!< The output source is set to Not Cam B Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO4_NotCamBPulseGenerator1,  //!< The output source is set to Not Cam B Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO4_NotCamBPulseGenerator2,  //!< The output source is set to Not Cam B Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO4_NotCamBPulseGenerator3,  //!< The output source is set to Not Cam B Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO4_NotCamCPulseGenerator0,  //!< The output source is set to Not Cam C Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO4_NotCamCPulseGenerator1,  //!< The output source is set to Not Cam C Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO4_NotCamCPulseGenerator2,  //!< The output source is set to Not Cam C Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO4_NotCamCPulseGenerator3,  //!< The output source is set to Not Cam C Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO4_NotCamDPulseGenerator0,  //!< The output source is set to Not Cam D Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO4_NotCamDPulseGenerator1,  //!< The output source is set to Not Cam D Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO4_NotCamDPulseGenerator2,  //!< The output source is set to Not Cam D Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO4_NotCamDPulseGenerator3,  //!< The output source is set to Not Cam D Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO4_NotPulseGenerator0,  //!< Inverted signal of pulse generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO4_NotPulseGenerator1,  //!< Inverted signal of pulse generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO4_NotPulseGenerator2,  //!< Inverted signal of pulse generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO4_NotPulseGenerator3,  //!< Inverted signal of pulse generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO4_PulseGenerator0,  //!< The source for the output statistics is set to Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO4_PulseGenerator1,  //!< The source for the output statistics is set to Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO4_PulseGenerator2,  //!< The source for the output statistics is set to Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO4_PulseGenerator3,  //!< The source for the output statistics is set to Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO4_VCC  //!< A static one - Applies to: CoaXPress
    };

    //! Valid values for TriggerOutSelectGPO5
    enum TriggerOutSelectGPO5Enums
    {
        TriggerOutSelectGPO5_BypassFrontGPI0,  //!< Bypass the Front GPI 0 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO5_BypassFrontGPI1,  //!< Bypass the Front GPI 1 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO5_BypassFrontGPI2,  //!< Bypass the Front GPI 2 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO5_BypassFrontGPI3,  //!< Bypass the Front GPI 3 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO5_BypassGPI0,  //!< Bypass the GPI 0 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO5_BypassGPI1,  //!< Bypass the GPI 1 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO5_BypassGPI2,  //!< Bypass the GPI 2 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO5_BypassGPI3,  //!< Bypass the GPI 3 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO5_BypassGPI4,  //!< Bypass the GPI 4 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO5_BypassGPI5,  //!< Bypass the GPI 5 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO5_BypassGPI6,  //!< Bypass the GPI 6 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO5_BypassGPI7,  //!< Bypass the GPI 7 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO5_CamAPulseGenerator0,  //!< Camera port A Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO5_CamAPulseGenerator1,  //!< Camera port A Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO5_CamAPulseGenerator2,  //!< Camera port A Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO5_CamAPulseGenerator3,  //!< Camera port A Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO5_CamBPulseGenerator0,  //!< The output source is set to Cam B Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO5_CamBPulseGenerator1,  //!< The output source is set to Cam B Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO5_CamBPulseGenerator2,  //!< The output source is set to Cam B Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO5_CamBPulseGenerator3,  //!< The output source is set to Cam B Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO5_CamCPulseGenerator0,  //!< The output source is set to Cam C Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO5_CamCPulseGenerator1,  //!< The output source is set to Cam C Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO5_CamCPulseGenerator2,  //!< The output source is set to Cam C Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO5_CamCPulseGenerator3,  //!< The output source is set to Cam C Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO5_CamDPulseGenerator0,  //!< The output source is set to Cam D Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO5_CamDPulseGenerator1,  //!< The output source is set to Cam D Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO5_CamDPulseGenerator2,  //!< The output source is set to Cam D Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO5_CamDPulseGenerator3,  //!< The output source is set to Cam D Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO5_GND,  //!< A static zero - Applies to: CoaXPress
        TriggerOutSelectGPO5_NotBypassFrontGPI0,  //!< The output source is set to Not Bypass Front-GPI 0 - Applies to: CoaXPress
        TriggerOutSelectGPO5_NotBypassFrontGPI1,  //!< The output source is set to Not Bypass Front-GPI 1 - Applies to: CoaXPress
        TriggerOutSelectGPO5_NotBypassFrontGPI2,  //!< The output source is set to Not Bypass Front-GPI 2 - Applies to: CoaXPress
        TriggerOutSelectGPO5_NotBypassFrontGPI3,  //!< The output source is set to Not Bypass Front-GPI 3 - Applies to: CoaXPress
        TriggerOutSelectGPO5_NotBypassGPI0,  //!< Bypass the inverted GPI 0 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO5_NotBypassGPI1,  //!< Bypass the inverted GPI 1 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO5_NotBypassGPI2,  //!< Bypass the inverted GPI 2 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO5_NotBypassGPI3,  //!< Bypass the inverted GPI 3 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO5_NotBypassGPI4,  //!< Bypass the inverted GPI 4 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO5_NotBypassGPI5,  //!< Bypass the inverted GPI 5 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO5_NotBypassGPI6,  //!< Bypass the inverted GPI 6 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO5_NotBypassGPI7,  //!< Bypass the inverted GPI 7 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO5_NotCamAPulseGenerator0,  //!< Inverted signal of camera port A Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO5_NotCamAPulseGenerator1,  //!< Inverted signal of camera port A Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO5_NotCamAPulseGenerator2,  //!< Inverted signal of camera port A Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO5_NotCamAPulseGenerator3,  //!< Inverted signal of camera port A Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO5_NotCamBPulseGenerator0,  //!< The output source is set to Not Cam B Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO5_NotCamBPulseGenerator1,  //!< The output source is set to Not Cam B Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO5_NotCamBPulseGenerator2,  //!< The output source is set to Not Cam B Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO5_NotCamBPulseGenerator3,  //!< The output source is set to Not Cam B Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO5_NotCamCPulseGenerator0,  //!< The output source is set to Not Cam C Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO5_NotCamCPulseGenerator1,  //!< The output source is set to Not Cam C Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO5_NotCamCPulseGenerator2,  //!< The output source is set to Not Cam C Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO5_NotCamCPulseGenerator3,  //!< The output source is set to Not Cam C Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO5_NotCamDPulseGenerator0,  //!< The output source is set to Not Cam D Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO5_NotCamDPulseGenerator1,  //!< The output source is set to Not Cam D Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO5_NotCamDPulseGenerator2,  //!< The output source is set to Not Cam D Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO5_NotCamDPulseGenerator3,  //!< The output source is set to Not Cam D Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO5_NotPulseGenerator0,  //!< Inverted signal of pulse generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO5_NotPulseGenerator1,  //!< Inverted signal of pulse generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO5_NotPulseGenerator2,  //!< Inverted signal of pulse generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO5_NotPulseGenerator3,  //!< Inverted signal of pulse generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO5_PulseGenerator0,  //!< The source for the output statistics is set to Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO5_PulseGenerator1,  //!< The source for the output statistics is set to Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO5_PulseGenerator2,  //!< The source for the output statistics is set to Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO5_PulseGenerator3,  //!< The source for the output statistics is set to Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO5_VCC  //!< A static one - Applies to: CoaXPress
    };

    //! Valid values for TriggerOutSelectGPO6
    enum TriggerOutSelectGPO6Enums
    {
        TriggerOutSelectGPO6_BypassFrontGPI0,  //!< Bypass the Front GPI 0 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO6_BypassFrontGPI1,  //!< Bypass the Front GPI 1 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO6_BypassFrontGPI2,  //!< Bypass the Front GPI 2 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO6_BypassFrontGPI3,  //!< Bypass the Front GPI 3 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO6_BypassGPI0,  //!< Bypass the GPI 0 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO6_BypassGPI1,  //!< Bypass the GPI 1 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO6_BypassGPI2,  //!< Bypass the GPI 2 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO6_BypassGPI3,  //!< Bypass the GPI 3 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO6_BypassGPI4,  //!< Bypass the GPI 4 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO6_BypassGPI5,  //!< Bypass the GPI 5 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO6_BypassGPI6,  //!< Bypass the GPI 6 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO6_BypassGPI7,  //!< Bypass the GPI 7 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO6_CamAPulseGenerator0,  //!< Camera port A Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO6_CamAPulseGenerator1,  //!< Camera port A Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO6_CamAPulseGenerator2,  //!< Camera port A Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO6_CamAPulseGenerator3,  //!< Camera port A Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO6_CamBPulseGenerator0,  //!< The output source is set to Cam B Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO6_CamBPulseGenerator1,  //!< The output source is set to Cam B Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO6_CamBPulseGenerator2,  //!< The output source is set to Cam B Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO6_CamBPulseGenerator3,  //!< The output source is set to Cam B Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO6_CamCPulseGenerator0,  //!< The output source is set to Cam C Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO6_CamCPulseGenerator1,  //!< The output source is set to Cam C Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO6_CamCPulseGenerator2,  //!< The output source is set to Cam C Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO6_CamCPulseGenerator3,  //!< The output source is set to Cam C Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO6_CamDPulseGenerator0,  //!< The output source is set to Cam D Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO6_CamDPulseGenerator1,  //!< The output source is set to Cam D Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO6_CamDPulseGenerator2,  //!< The output source is set to Cam D Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO6_CamDPulseGenerator3,  //!< The output source is set to Cam D Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO6_GND,  //!< A static zero - Applies to: CoaXPress
        TriggerOutSelectGPO6_NotBypassFrontGPI0,  //!< The output source is set to Not Bypass Front-GPI 0 - Applies to: CoaXPress
        TriggerOutSelectGPO6_NotBypassFrontGPI1,  //!< The output source is set to Not Bypass Front-GPI 1 - Applies to: CoaXPress
        TriggerOutSelectGPO6_NotBypassFrontGPI2,  //!< The output source is set to Not Bypass Front-GPI 2 - Applies to: CoaXPress
        TriggerOutSelectGPO6_NotBypassFrontGPI3,  //!< The output source is set to Not Bypass Front-GPI 3 - Applies to: CoaXPress
        TriggerOutSelectGPO6_NotBypassGPI0,  //!< Bypass the inverted GPI 0 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO6_NotBypassGPI1,  //!< Bypass the inverted GPI 1 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO6_NotBypassGPI2,  //!< Bypass the inverted GPI 2 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO6_NotBypassGPI3,  //!< Bypass the inverted GPI 3 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO6_NotBypassGPI4,  //!< Bypass the inverted GPI 4 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO6_NotBypassGPI5,  //!< Bypass the inverted GPI 5 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO6_NotBypassGPI6,  //!< Bypass the inverted GPI 6 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO6_NotBypassGPI7,  //!< Bypass the inverted GPI 7 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO6_NotCamAPulseGenerator0,  //!< Inverted signal of camera port A Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO6_NotCamAPulseGenerator1,  //!< Inverted signal of camera port A Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO6_NotCamAPulseGenerator2,  //!< Inverted signal of camera port A Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO6_NotCamAPulseGenerator3,  //!< Inverted signal of camera port A Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO6_NotCamBPulseGenerator0,  //!< The output source is set to Not Cam B Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO6_NotCamBPulseGenerator1,  //!< The output source is set to Not Cam B Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO6_NotCamBPulseGenerator2,  //!< The output source is set to Not Cam B Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO6_NotCamBPulseGenerator3,  //!< The output source is set to Not Cam B Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO6_NotCamCPulseGenerator0,  //!< The output source is set to Not Cam C Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO6_NotCamCPulseGenerator1,  //!< The output source is set to Not Cam C Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO6_NotCamCPulseGenerator2,  //!< The output source is set to Not Cam C Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO6_NotCamCPulseGenerator3,  //!< The output source is set to Not Cam C Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO6_NotCamDPulseGenerator0,  //!< The output source is set to Not Cam D Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO6_NotCamDPulseGenerator1,  //!< The output source is set to Not Cam D Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO6_NotCamDPulseGenerator2,  //!< The output source is set to Not Cam D Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO6_NotCamDPulseGenerator3,  //!< The output source is set to Not Cam D Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO6_NotPulseGenerator0,  //!< Inverted signal of pulse generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO6_NotPulseGenerator1,  //!< Inverted signal of pulse generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO6_NotPulseGenerator2,  //!< Inverted signal of pulse generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO6_NotPulseGenerator3,  //!< Inverted signal of pulse generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO6_PulseGenerator0,  //!< The source for the output statistics is set to Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO6_PulseGenerator1,  //!< The source for the output statistics is set to Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO6_PulseGenerator2,  //!< The source for the output statistics is set to Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO6_PulseGenerator3,  //!< The source for the output statistics is set to Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO6_VCC  //!< A static one - Applies to: CoaXPress
    };

    //! Valid values for TriggerOutSelectGPO7
    enum TriggerOutSelectGPO7Enums
    {
        TriggerOutSelectGPO7_BypassFrontGPI0,  //!< Bypass the Front GPI 0 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO7_BypassFrontGPI1,  //!< Bypass the Front GPI 1 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO7_BypassFrontGPI2,  //!< Bypass the Front GPI 2 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO7_BypassFrontGPI3,  //!< Bypass the Front GPI 3 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO7_BypassGPI0,  //!< Bypass the GPI 0 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO7_BypassGPI1,  //!< Bypass the GPI 1 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO7_BypassGPI2,  //!< Bypass the GPI 2 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO7_BypassGPI3,  //!< Bypass the GPI 3 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO7_BypassGPI4,  //!< Bypass the GPI 4 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO7_BypassGPI5,  //!< Bypass the GPI 5 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO7_BypassGPI6,  //!< Bypass the GPI 6 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO7_BypassGPI7,  //!< Bypass the GPI 7 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO7_CamAPulseGenerator0,  //!< Camera port A Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO7_CamAPulseGenerator1,  //!< Camera port A Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO7_CamAPulseGenerator2,  //!< Camera port A Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO7_CamAPulseGenerator3,  //!< Camera port A Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO7_CamBPulseGenerator0,  //!< The output source is set to Cam B Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO7_CamBPulseGenerator1,  //!< The output source is set to Cam B Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO7_CamBPulseGenerator2,  //!< The output source is set to Cam B Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO7_CamBPulseGenerator3,  //!< The output source is set to Cam B Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO7_CamCPulseGenerator0,  //!< The output source is set to Cam C Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO7_CamCPulseGenerator1,  //!< The output source is set to Cam C Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO7_CamCPulseGenerator2,  //!< The output source is set to Cam C Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO7_CamCPulseGenerator3,  //!< The output source is set to Cam C Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO7_CamDPulseGenerator0,  //!< The output source is set to Cam D Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO7_CamDPulseGenerator1,  //!< The output source is set to Cam D Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO7_CamDPulseGenerator2,  //!< The output source is set to Cam D Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO7_CamDPulseGenerator3,  //!< The output source is set to Cam D Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO7_GND,  //!< A static zero - Applies to: CoaXPress
        TriggerOutSelectGPO7_NotBypassFrontGPI0,  //!< The output source is set to Not Bypass Front-GPI 0 - Applies to: CoaXPress
        TriggerOutSelectGPO7_NotBypassFrontGPI1,  //!< The output source is set to Not Bypass Front-GPI 1 - Applies to: CoaXPress
        TriggerOutSelectGPO7_NotBypassFrontGPI2,  //!< The output source is set to Not Bypass Front-GPI 2 - Applies to: CoaXPress
        TriggerOutSelectGPO7_NotBypassFrontGPI3,  //!< The output source is set to Not Bypass Front-GPI 3 - Applies to: CoaXPress
        TriggerOutSelectGPO7_NotBypassGPI0,  //!< Bypass the inverted GPI 0 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO7_NotBypassGPI1,  //!< Bypass the inverted GPI 1 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO7_NotBypassGPI2,  //!< Bypass the inverted GPI 2 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO7_NotBypassGPI3,  //!< Bypass the inverted GPI 3 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO7_NotBypassGPI4,  //!< Bypass the inverted GPI 4 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO7_NotBypassGPI5,  //!< Bypass the inverted GPI 5 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO7_NotBypassGPI6,  //!< Bypass the inverted GPI 6 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO7_NotBypassGPI7,  //!< Bypass the inverted GPI 7 signal directly to the output - Applies to: CoaXPress
        TriggerOutSelectGPO7_NotCamAPulseGenerator0,  //!< Inverted signal of camera port A Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO7_NotCamAPulseGenerator1,  //!< Inverted signal of camera port A Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO7_NotCamAPulseGenerator2,  //!< Inverted signal of camera port A Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO7_NotCamAPulseGenerator3,  //!< Inverted signal of camera port A Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO7_NotCamBPulseGenerator0,  //!< The output source is set to Not Cam B Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO7_NotCamBPulseGenerator1,  //!< The output source is set to Not Cam B Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO7_NotCamBPulseGenerator2,  //!< The output source is set to Not Cam B Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO7_NotCamBPulseGenerator3,  //!< The output source is set to Not Cam B Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO7_NotCamCPulseGenerator0,  //!< The output source is set to Not Cam C Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO7_NotCamCPulseGenerator1,  //!< The output source is set to Not Cam C Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO7_NotCamCPulseGenerator2,  //!< The output source is set to Not Cam C Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO7_NotCamCPulseGenerator3,  //!< The output source is set to Not Cam C Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO7_NotCamDPulseGenerator0,  //!< The output source is set to Not Cam D Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO7_NotCamDPulseGenerator1,  //!< The output source is set to Not Cam D Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO7_NotCamDPulseGenerator2,  //!< The output source is set to Not Cam D Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO7_NotCamDPulseGenerator3,  //!< The output source is set to Not Cam D Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO7_NotPulseGenerator0,  //!< Inverted signal of pulse generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO7_NotPulseGenerator1,  //!< Inverted signal of pulse generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO7_NotPulseGenerator2,  //!< Inverted signal of pulse generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO7_NotPulseGenerator3,  //!< Inverted signal of pulse generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO7_PulseGenerator0,  //!< The source for the output statistics is set to Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutSelectGPO7_PulseGenerator1,  //!< The source for the output statistics is set to Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutSelectGPO7_PulseGenerator2,  //!< The source for the output statistics is set to Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutSelectGPO7_PulseGenerator3,  //!< The source for the output statistics is set to Pulse Generator 3 - Applies to: CoaXPress
        TriggerOutSelectGPO7_VCC  //!< A static one - Applies to: CoaXPress
    };

    //! Valid values for TriggerOutStatisticsSource
    enum TriggerOutStatisticsSourceEnums
    {
        TriggerOutStatisticsSource_PulseGenerator0,  //!< The source for the output statistics is set to Pulse Generator 0 - Applies to: CoaXPress
        TriggerOutStatisticsSource_PulseGenerator1,  //!< The source for the output statistics is set to Pulse Generator 1 - Applies to: CoaXPress
        TriggerOutStatisticsSource_PulseGenerator2,  //!< The source for the output statistics is set to Pulse Generator 2 - Applies to: CoaXPress
        TriggerOutStatisticsSource_PulseGenerator3  //!< The source for the output statistics is set to Pulse Generator 3 - Applies to: CoaXPress
    };

    //! Valid values for TriggerQueueMode
    enum TriggerQueueModeEnums
    {
        TriggerQueueMode_Off,  //!< The trigger queue mode is disabled - Applies to: CoaXPress
        TriggerQueueMode_On  //!< The trigger queue mode is enabled - Applies to: CoaXPress
    };

    //! Valid values for TriggerState
    enum TriggerStateEnums
    {
        TriggerState_Active,  //!< The trigger state is set to Active - Applies to: CoaXPress
        TriggerState_AsyncStop,  //!< The trigger state is set to AsyncStop - Applies to: CoaXPress
        TriggerState_SyncStop  //!< The trigger state is set to SyncStop - Applies to: CoaXPress
    };


    
    
    //**************************************************************************************************
    // Parameter class CUniversalTLParams_Params_v6_3_0
    //**************************************************************************************************
    

    /*!
    \brief A parameter class containing all parameters as members that are available for pylon device transport layers

    The parameter class is used by the \c Pylon::CBaslerUniversalInstantCamera class.
    The \ref sample_ParametrizeCamera_NativeParameterAccess code sample shows how to access camera parameters via the \c Pylon::CBaslerUniversalInstantCamera class.
    */
    class PYLONBASE_API CUniversalTLParams_Params_v6_3_0
    {
    //----------------------------------------------------------------------------------------------------------------
    // Implementation
    //----------------------------------------------------------------------------------------------------------------
    protected:
    // If you want to show the following methods in the help file
    // add the string HIDE_CLASS_METHODS to the ENABLED_SECTIONS tag in the doxygen file
    //! \cond HIDE_CLASS_METHODS
        
        //! Constructor
        CUniversalTLParams_Params_v6_3_0( void );

        //! Destructor
        ~CUniversalTLParams_Params_v6_3_0( void );

        //! Initializes the references
        void _Initialize( GENAPI_NAMESPACE::INodeMap* );

    //! \endcond

    private:
        class CUniversalTLParams_Params_v6_3_0_Data;
        CUniversalTLParams_Params_v6_3_0_Data* m_pCUniversalTLParams_Params_v6_3_0_Data;


    //----------------------------------------------------------------------------------------------------------------
    // References to features
    //----------------------------------------------------------------------------------------------------------------
    public:
        //! \name Categories: Trigger
        //@{
        /*!
            \brief Sets the trigger mode of the area trigger signal - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=AreaTriggerMode" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IEnumParameterT<AreaTriggerModeEnums>& AreaTriggerMode;

        //@}


        //! \name Categories: ImageFormatControl
        //@{
        /*!
            \brief Enables/disables automatic control of the pixel format in the device - Applies to: CoaXPress

            Enables/disables automatic control of the pixel format in the device. If enabled, the frame grabber's pixel format and its dependencies are automatically updated to match the camera's pixel format setting.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=AutomaticFormatControl" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IBooleanEx& AutomaticFormatControl;

        //@}


        //! \name Categories: ImageFormatControl
        //@{
        /*!
            \brief Activates or deactivates the automatic control of the ROI in the Device - Applies to: CoaXPress

            Activates or deactivates the automatic control of the ROI in the Device. If this feature is active, the frame grabber ROI is automatically updated to the camera one.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=AutomaticROIControl" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IBooleanEx& AutomaticROIControl;

        //@}


        //! \name Categories: OutputFormat
        //@{
        /*!
            \brief Sets the alignment of the bits in the output format - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=BitAlignment" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IEnumParameterT<BitAlignmentEnums>& BitAlignment;

        //@}


        //! \name Categories: ExtendedDeviceControl and Root
        //@{
        /*!
            \brief Enables sending all commands and receiving all acknowledges twice - Applies to: GigE and blaze

            Enables sending all commands and receiving all acknowledges twice. This option should only be enabled in case of network problems.

            Visibility: Guru

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=CommandDuplicationEnable" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IBooleanEx& CommandDuplicationEnable;

        //@}


        //! \name Categories: CoaXPress
        //@{
        /*!
            \brief Number of errors corrected - Applies to: CoaXPress

            Numbers of errors corrected. This parameter indicates single-byte error correction in CXP stream packets.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=CorrectedErrorCount" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& CorrectedErrorCount;

        //@}


        //! \name Categories: OutputFormat
        //@{
        /*!
            \brief User-defined bit shift to the right - Applies to: CoaXPress

            User-defined bit shift to the right. This allows you to shift the pixel data in the output format to the right by a value of your choice. The Alignment parameter has to be set to CustomBitShift for this.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=CustomBitShiftRight" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& CustomBitShiftRight;

        //@}


        //! \name Categories: TransportLayerControl
        //@{
        /*!
            \brief Sets the link configuration for the communication between the receiver and transmitter device - Applies to: CoaXPress


            Visibility: Beginner

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=CxpLinkConfiguration" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IEnumParameterT<CxpLinkConfigurationEnums>& CxpLinkConfiguration;

        //@}


        //! \name Categories: CoaXPress
        //@{
        /*!
            \brief Sets the trigger packet mode - Applies to: CoaXPress

            Sets the trigger packet mode. This parameter allows you to send triggers on rising edges only. It can be used for extreme trigger rates. This parameter may violate the CXP standard.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=CxpTriggerPacketMode" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IEnumParameterT<CxpTriggerPacketModeEnums>& CxpTriggerPacketMode;

        //@}


        //! \name Categories: DeviceInformation
        //@{
        /*!
            \brief Returns the device's access status at the moment when the Device Update List command was last executed - Applies to: CoaXPress and blaze

            Returns the device's access status at the moment when the Device Update List command was last executed. This value only changes when the Device Update List command is executed.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=DeviceAccessStatus" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IEnumParameterT<DeviceAccessStatusEnums>& DeviceAccessStatus;

        //@}


        //! \name Categories: DeviceInformation
        //@{
        /*!
            \brief User-friendly name of the device - Applies to: CoaXPress and blaze


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=DeviceDisplayName" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IStringEx& DeviceDisplayName;

        //@}


        //! \name Categories: DeviceControl
        //@{
        /*!
            \brief Sets the endianness handling mode - Applies to: blaze


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=DeviceEndianessMechanism" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IEnumParameterT<DeviceEndianessMechanismEnums>& DeviceEndianessMechanism;

        //@}


        //! \name Categories: DeviceInformation
        //@{
        /*!
            \brief Interface-wide unique ID of the selected device - Applies to: CoaXPress and blaze

            Interface-wide unique ID of the selected device. This value only changes when the Device Update List command is executed.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=DeviceID" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IStringEx& DeviceID;

        //@}


        //! \name Categories: DeviceInformation
        //@{
        /*!
            \brief Additional information about the manufacturer of the device - Applies to: blaze


            Visibility: Beginner

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=DeviceManufacturerInfo" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IStringEx& DeviceManufacturerInfo;

        //@}


        //! \name Categories: DeviceInformation
        //@{
        /*!
            \brief Name of the device model - Applies to: CoaXPress and blaze

            Name of the device model. This value only changes when the Device Update List command is executed.

            Visibility: Beginner

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=DeviceModelName" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IStringEx& DeviceModelName;

        //@}


        //! \name Categories: DeviceInformation
        //@{
        /*!
            \brief Serial number of the remote device - Applies to: blaze

            Serial number of the remote device. This value only changes when the Device Update List command is executed.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=DeviceSerialNumber" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IStringEx& DeviceSerialNumber;

        //@}


        //! \name Categories: DeviceInformation
        //@{
        /*!
            \brief Returns the transport layer of the device - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=DeviceType" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IEnumParameterT<DeviceTypeEnums>& DeviceType;

        //@}


        //! \name Categories: DeviceInformation
        //@{
        /*!
            \brief Name of the device vendor - Applies to: CoaXPress and blaze

            Name of the device vendor. This value only changes when the Device Update List command is executed.

            Visibility: Beginner

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=DeviceVendorName" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IStringEx& DeviceVendorName;

        //@}


        //! \name Categories: EventDeviceLostData
        //@{
        /*!
            \brief Returns the unique identifier of the Device Lost event - Applies to: CoaXPress and blaze

            Returns the unique Identifier of the Device Lost event.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=EventDeviceLost" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& EventDeviceLost;

        //@}


        //! \name Categories: EventControl
        //@{
        /*!
            \brief Activate or deactivate the notification to the host application of the occurrence of the selected Event - Applies to: CoaXPress and blaze


            Visibility: Expert

            Selected by: EventSelector

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=EventNotification" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IEnumParameterT<EventNotificationEnums>& EventNotification;

        //@}


        //! \name Categories: EventControl
        //@{
        /*!
            \brief Sets which event to signal to the host application - Applies to: CoaXPress and blaze


            Visibility: Expert

            Selecting Parameters: EventNotification

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=EventSelector" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IEnumParameterT<EventSelectorEnums>& EventSelector;

        //@}


        //! \name Categories: BufferStatus
        //@{
        /*!
            \brief Current buffer fill level - Applies to: CoaXPress

            Current buffer fill level. This value allows you to check whether the average input bandwidth of the camera is too high to be processed by the applet.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=FillLevel" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& FillLevel;

        //@}


        //! \name Categories: OutputFormat
        //@{
        /*!
            \brief Sets the pixel format of the image that is output to the computer - Applies to: CoaXPress

            Sets the pixel format of the image that is output to the computer. The data is converted from the internal bit width and color format to the output format selected. This setting is not related to the pixel format input from the camera sensor. For more information, see the Pixel Format parameter documentation.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=Format" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IEnumParameterT<FormatEnums>& Format;

        //@}


        //! \name Categories: External
        //@{
        /*!
            \brief Current input signal levels of all front GPI inputs (bit mask) - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=FrontGPI" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& FrontGPI;

        //@}


        //! \name Categories: External
        //@{
        /*!
            \brief GPI State - Applies to: CoaXPress

            Current input signal levels of all GPI inputs (Bitmask).

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=GPI" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& GPI;

        //@}


        //! \name Categories: Gentl
        //@{
        /*!
            \brief  Applies to: CoaXPress

            Ignore the FG output format and manage it internally in the GenTL producer

            Visibility: Beginner

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=GentlInfoIgnorefgformat" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IStringEx& GentlInfoIgnorefgformat;

        //@}


        //! \name Categories: Gentl
        //@{
        /*!
            \brief  Applies to: CoaXPress

            Version of the GenTL description Interface in the applet.

            Visibility: Beginner

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=GentlInfoVersion" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IStringEx& GentlInfoVersion;

        //@}


        //! \name Categories: DeviceInformation
        //@{
        /*!
            \brief Current gateway IP address of the GVCP interface of the remote device - Applies to: blaze


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=GevDeviceGateway" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& GevDeviceGateway;

        //@}


        //! \name Categories: DeviceInformation
        //@{
        /*!
            \brief Current IP address of the GVCP interface of the remote device - Applies to: blaze


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=GevDeviceIPAddress" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& GevDeviceIPAddress;

        //@}


        //! \name Categories: DeviceInformation
        //@{
        /*!
            \brief 48-bit MAC address of the GVCP interface of the remote device - Applies to: blaze


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=GevDeviceMACAddress" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& GevDeviceMACAddress;

        //@}


        //! \name Categories: DeviceInformation
        //@{
        /*!
            \brief Current subnet mask of the GVCP interface of the remote device - Applies to: blaze


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=GevDeviceSubnetMask" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& GevDeviceSubnetMask;

        //@}


        //! \name Categories: Root
        //@{
        /*!
            \brief Heartbeat timeout value on the host side in milliseconds - Applies to: GigE

            Heartbeat timeout value on the host side in milliseconds. This parameter is linked with the Heartbeat Timeout camera parameter. If the camera parameter changes, the host parameter changes accordingly, and vice versa.

            Visibility: Beginner

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=HeartbeatTimeout" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& HeartbeatTimeout;

        //@}


        //! \name Categories: ROI
        //@{
        /*!
            \brief Height of the image ROI on the host side - Applies to: CoaXPress

            Height of the image ROI on the host side. This parameter is linked with the Height camera parameter. If the camera parameter changes, the host parameter changes accordingly, but not vice versa. The host parameter should not be set to a value different from the camera parameter.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=Height" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& Height;

        //@}


        //! \name Categories: ImageSelector
        //@{
        /*!
            \brief Defines which image in a sequence is grabbed - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=ImageSelect" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& ImageSelect;

        //@}


        //! \name Categories: ImageSelector
        //@{
        /*!
            \brief Select the length of an image sequence - Applies to: CoaXPress

            Select the length of an image sequence. Only a single image of the sequence is grabbed.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=ImageSelectPeriod" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& ImageSelectPeriod;

        //@}


        //! \name Categories: DeviceControl
        //@{
        /*!
            \brief Specifies maximum number of tries before failing the control channel commands - Applies to: blaze


            Visibility: Guru

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=LinkCommandRetryCount" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& LinkCommandRetryCount;

        //@}


        //! \name Categories: DeviceControl
        //@{
        /*!
            \brief Specifies application timeout for the control channel communication - Applies to: blaze

            Specifies application timeout for the control channel communication. Up to DeviceLinkCommandRetryCount attempts with this timeout are made before a command fails with a timeout error.

            Visibility: Beginner

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=LinkCommandTimeout" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IFloatEx& LinkCommandTimeout;

        //@}


        //! \name Categories: LookupTable
        //@{
        /*!
            \brief Filename with custom lookup table values - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=LutCustomFile" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IStringEx& LutCustomFile;

        //@}


        //! \name Categories: LookupTable
        //@{
        /*!
            \brief Switch the LUT and processing functionality on or off - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=LutEnable" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IEnumParameterT<LutEnableEnums>& LutEnable;

        //@}


        //! \name Categories: AppletProperties
        //@{
        /*!
            \brief Type of lookup table implementation - Applies to: CoaXPress

            Type of LUT implementation.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=LutImplementationType" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IEnumParameterT<LutImplementationTypeEnums>& LutImplementationType;

        //@}


        //! \name Categories: AppletProperties
        //@{
        /*!
            \brief Pixel bit depth at the lookup table input - Applies to: CoaXPress

            Pixel bit depth at LUT input.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=LutInputPixelBitDepth" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& LutInputPixelBitDepth;

        //@}


        //! \name Categories: AppletProperties
        //@{
        /*!
            \brief Pixel bit depth at the lookup table output - Applies to: CoaXPress

            Pixel bit depth at LUT output.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=LutOutputPixelBitDepth" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& LutOutputPixelBitDepth;

        //@}


        //! \name Categories: LookupTable
        //@{
        /*!
            \brief Save current lookup table configuration - Applies to: CoaXPress

            Save current LUT configuration.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=LutSaveFile" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IStringEx& LutSaveFile;

        //@}


        //! \name Categories: LookupTable
        //@{
        /*!
            \brief Set the type of the lookup table: Custom values or applet processor - Applies to: CoaXPress

            Enables the LUT to be loaded with custom values or uses the applet's processor.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=LutType" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IEnumParameterT<LutTypeEnums>& LutType;

        //@}


        //! \name Categories: LookupTable
        //@{
        /*!
            \brief Field with lookup table values - Applies to: CoaXPress

            Field with LUT Values.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=LutValue" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& LutValue;

        //@}


        //! \name Categories: LookupTable
        //@{
        /*!
            \brief Field with blue lookup table values - Applies to: CoaXPress

            Field with blue LUT Values.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=LutValueBlue" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& LutValueBlue;

        //@}


        //! \name Categories: LookupTable
        //@{
        /*!
            \brief  Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=LutValueBlueSelector" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& LutValueBlueSelector;

        //@}


        //! \name Categories: LookupTable
        //@{
        /*!
            \brief Field with green looup table values - Applies to: CoaXPress

            Field with green LUT Values.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=LutValueGreen" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& LutValueGreen;

        //@}


        //! \name Categories: LookupTable
        //@{
        /*!
            \brief  Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=LutValueGreenSelector" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& LutValueGreenSelector;

        //@}


        //! \name Categories: LookupTable
        //@{
        /*!
            \brief Field with red lookup table values - Applies to: CoaXPress

            Field with red LUT Values.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=LutValueRed" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& LutValueRed;

        //@}


        //! \name Categories: LookupTable
        //@{
        /*!
            \brief  Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=LutValueRedSelector" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& LutValueRedSelector;

        //@}


        //! \name Categories: LookupTable
        //@{
        /*!
            \brief  Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=LutValueSelector" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& LutValueSelector;

        //@}


        //! \name Categories: Root
        //@{
        /*!
            \brief Maximum number of retries for read operations after a read operation has timed out - Applies to: GigE


            Visibility: Guru

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=MaxRetryCountRead" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& MaxRetryCountRead;

        //@}


        //! \name Categories: Root
        //@{
        /*!
            \brief Maximum number of retries for write operations after a write operation has timed out - Applies to: GigE


            Visibility: Guru

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=MaxRetryCountWrite" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& MaxRetryCountWrite;

        //@}


        //! \name Categories: Root
        //@{
        /*!
            \brief Enables mapping of certain SFNC 1 x node names to SFNC 2 x node names - Applies to: GigE and USB

            Enables mapping of certain SFNC 1.x node names to SFNC 2.x node names. This allows you to write code for camera devices that are compatible with different SFNC (Standard Features Naming Convention) versions. Available for USB camera devices only.

            Visibility: Guru

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=MigrationModeEnable" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IBooleanEx& MigrationModeEnable;

        //@}


        //! \name Categories: OutStatistics
        //@{
        /*!
            \brief Returns whether missing frames are reported - Applies to: CoaXPress

            Returns whether missing frames are reported. If no frame is received for a trigger pulse, the parameter is set to Yes.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=MissingCameraFrameResponse" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IEnumParameterT<MissingCameraFrameResponseEnums>& MissingCameraFrameResponse;

        //@}


        //! \name Categories: OutStatistics
        //@{
        /*!
            \brief Clears the Missing Camera Frame Response parameter - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=MissingCameraFrameResponseClear" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::ICommandEx& MissingCameraFrameResponseClear;

        //@}


        //! \name Categories: ROI
        //@{
        /*!
            \brief Acquisition ROI x-offset, independent of camera ROI - Applies to: CoaXPress

            X-offset of the acquisition ROI independent of the camera ROI.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=OffsetX" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& OffsetX;

        //@}


        //! \name Categories: ROI
        //@{
        /*!
            \brief Acquisition ROI y-offset, independent of camera ROI - Applies to: CoaXPress

            Y-offset of the acquisition ROI independent of the camera ROI.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=OffsetY" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& OffsetY;

        //@}


        //! \name Categories: ImageFormatControl
        //@{
        /*!
            \brief Automatically outputs packed formats instead of unpacked ones - Applies to: CoaXPress

            Automatically outputs packed formats instead of unpacked ones. This parameter is only available if the Automatic Format Control parameter is enabled.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=OutputPackedFormats" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IBooleanEx& OutputPackedFormats;

        //@}


        //! \name Categories: BufferStatus
        //@{
        /*!
            \brief Indicates whether the image buffer is currently in overflow state - Applies to: CoaXPress

            Indicates whether if the image buffer is currently in overflow state. An overflow results in the loss of images. The parameter is reset at each readout cycle.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=Overflow" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& Overflow;

        //@}


        //! \name Categories: CoaXPress
        //@{
        /*!
            \brief Number of packet errors - Applies to: CoaXPress

            Number of packet errors. This parameter indicates that CXP stream packet are lost.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=PacketTagErrorCount" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& PacketTagErrorCount;

        //@}


        //! \name Categories: OutputFormat
        //@{
        /*!
            \brief Applet-internal processing bit depth - Applies to: CoaXPress

            Applet-internal processing bit depth. This is the maximum bit depth at which pixels are processed. If you select a highter output and input bit depth, data may be truncated.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=PixelDepth" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& PixelDepth;

        //@}


        //! \name Categories: CoaXPress
        //@{
        /*!
            \brief Sets the format of the pixel data transmitted by the camera - Applies to: CoaXPress

            Sets the format of the pixel data transmitted by the camera. This setting has to match the pixel format set in the camera.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=PixelFormat" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IEnumParameterT<PixelFormatEnums>& PixelFormat;

        //@}


        //! \name Categories: Processing
        //@{
        /*!
            \brief Gain correction value - Applies to: CoaXPress

            Gain correction value. Available when LUT functionality is enabled.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=ProcessingGain" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IFloatEx& ProcessingGain;

        //@}


        //! \name Categories: Processing
        //@{
        /*!
            \brief Gamma correction value - Applies to: CoaXPress

            Gamma correction value. Available when LUT functionality is enabled.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=ProcessingGamma" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IFloatEx& ProcessingGamma;

        //@}


        //! \name Categories: Processing
        //@{
        /*!
            \brief Invert output - Applies to: CoaXPress

            Invert output. Available when LUT functionality is enabled.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=ProcessingInvert" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IEnumParameterT<ProcessingInvertEnums>& ProcessingInvert;

        //@}


        //! \name Categories: Processing
        //@{
        /*!
            \brief Offset correction value - Applies to: CoaXPress

            Offset correction value. Available when LUT functionality is enabled.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=ProcessingOffset" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IFloatEx& ProcessingOffset;

        //@}


        //! \name Categories: Root
        //@{
        /*!
            \brief Read access timeout value in milliseconds - Applies to: GigE


            Visibility: Beginner

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=ReadTimeout" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& ReadTimeout;

        //@}


        //! \name Categories: WhiteBalance
        //@{
        /*!
            \brief Blue gain of the white balancing - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=ScalingFactorBlue" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IFloatEx& ScalingFactorBlue;

        //@}


        //! \name Categories: WhiteBalance
        //@{
        /*!
            \brief Green gain of the white balancing - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=ScalingFactorGreen" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IFloatEx& ScalingFactorGreen;

        //@}


        //! \name Categories: WhiteBalance
        //@{
        /*!
            \brief Red gain of the white balancing - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=ScalingFactorRed" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IFloatEx& ScalingFactorRed;

        //@}


        //! \name Categories: SoftwareTrigger
        //@{
        /*!
            \brief Sends a software trigger pulse - Applies to: CoaXPress

            Sends a software trigger pulse or adds pulses to the trigger queue if it is enabled.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=SendSoftwareTrigger" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::ICommandEx& SendSoftwareTrigger;

        //@}


        //! \name Categories: SensorGeometry
        //@{
        /*!
            \brief Height of the sensor - Applies to: CoaXPress

            Sensor Height in selected tap geometry. Value is ignored if vantage point = TopLeft. Available DRAM and SensorWidth limit the maximum SensorHeight.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=SensorHeight" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& SensorHeight;

        //@}


        //! \name Categories: SensorGeometry
        //@{
        /*!
            \brief Width of the sensor - Applies to: CoaXPress

            Sensor width in selected tap geometry. Value is ignored if vantage point = Top-Left. Available DRAM and SensorHeight limit the maximum SensorWidth.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=SensorWidth" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& SensorWidth;

        //@}


        //! \name Categories: SoftwareTrigger
        //@{
        /*!
            \brief Indicates whether the software trigger is busy - Applies to: CoaXPress

            Indicates whether the software trigger is busy. This allows you to find out whether the trigger system is still processing software trigger pulses.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=SoftwareTriggerIsBusy" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IEnumParameterT<SoftwareTriggerIsBusyEnums>& SoftwareTriggerIsBusy;

        //@}


        //! \name Categories: SoftwareTrigger
        //@{
        /*!
            \brief Number of pulses in queue to be processed - Applies to: CoaXPress

            Number of pulses in queue to be processed. The Queue parameter needs to be enabled for this.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=SoftwareTriggerQueueFillLevel" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& SoftwareTriggerQueueFillLevel;

        //@}


        //! \name Categories: ExtendedDeviceControl and Root
        //@{
        /*!
            \brief Number of timeouts during read and write operations when waiting for a response from the device - Applies to: GigE and blaze


            Visibility: Guru

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=StatisticReadWriteTimeoutCount" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& StatisticReadWriteTimeoutCount;

        //@}


        //! \name Categories: Statistic
        //@{
        /*!
            \brief Last error status of a read or write operation - Applies to: BCON and USB


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=Statistic_Last_Error_Status" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& Statistic_Last_Error_Status;

        //@}


        //! \name Categories: Statistic
        //@{
        /*!
            \brief Last error status of a read or write operation - Applies to: BCON and USB


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=Statistic_Last_Error_Status_Text" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IStringEx& Statistic_Last_Error_Status_Text;

        //@}


        //! \name Categories: Statistic
        //@{
        /*!
            \brief Number of failed read operations - Applies to: BCON and USB


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=Statistic_Read_Operations_Failed_Count" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& Statistic_Read_Operations_Failed_Count;

        //@}


        //! \name Categories: Statistic
        //@{
        /*!
            \brief Number of read pipe resets - Applies to: USB


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=Statistic_Read_Pipe_Reset_Count" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& Statistic_Read_Pipe_Reset_Count;

        //@}


        //! \name Categories: Statistic
        //@{
        /*!
            \brief Number of failed write operations - Applies to: BCON and USB


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=Statistic_Write_Operations_Failed_Count" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& Statistic_Write_Operations_Failed_Count;

        //@}


        //! \name Categories: Statistic
        //@{
        /*!
            \brief Number of write pipe resets - Applies to: USB


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=Statistic_Write_Pipe_Reset_Count" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& Statistic_Write_Pipe_Reset_Count;

        //@}


        //! \name Categories: StreamEnumeration
        //@{
        /*!
            \brief User-friendly name of the selected stream - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=StreamDisplayName" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IStringEx& StreamDisplayName;

        //@}


        //! \name Categories: StreamEnumeration
        //@{
        /*!
            \brief Device-wide unique ID of the data stream - Applies to: CoaXPress and blaze


            Visibility: Beginner

            Selected by: StreamSelector

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=StreamID" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IStringEx& StreamID;

        //@}


        //! \name Categories: StreamEnumeration
        //@{
        /*!
            \brief Sets the stream channel - Applies to: CoaXPress and blaze

            Sets the stream channel. The parameter is 0-based in order to match the index of the C interface.

            Visibility: Beginner

            Selecting Parameters: StreamID

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=StreamSelector" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& StreamSelector;

        //@}


        //! \name Categories: Miscellaneous
        //@{
        /*!
            \brief  Applies to: CoaXPress

            Byte Alignment 8b10b locked

            Visibility: Beginner

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=SystemmonitorByteAlignment8b10bLocked" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IEnumParameterT<SystemmonitorByteAlignment8b10bLockedEnums>& SystemmonitorByteAlignment8b10bLocked;

        //@}


        //! \name Categories: Miscellaneous
        //@{
        /*!
            \brief  Applies to: CoaXPress

            Returns the Channel Current.

            Visibility: Beginner

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=SystemmonitorChannelCurrent" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IFloatEx& SystemmonitorChannelCurrent;

        //@}


        //! \name Categories: Miscellaneous
        //@{
        /*!
            \brief  Applies to: CoaXPress


            Visibility: Beginner

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=SystemmonitorChannelCurrentSelector" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& SystemmonitorChannelCurrentSelector;

        //@}


        //! \name Categories: Miscellaneous
        //@{
        /*!
            \brief  Applies to: CoaXPress

            Returns the Channel Voltage.

            Visibility: Beginner

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=SystemmonitorChannelVoltage" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IFloatEx& SystemmonitorChannelVoltage;

        //@}


        //! \name Categories: Miscellaneous
        //@{
        /*!
            \brief  Applies to: CoaXPress


            Visibility: Beginner

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=SystemmonitorChannelVoltageSelector" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& SystemmonitorChannelVoltageSelector;

        //@}


        //! \name Categories: Miscellaneous
        //@{
        /*!
            \brief  Applies to: CoaXPress

            Returns the current PCIe link speed in Gibibyte (2^30 byte).

            Visibility: Beginner

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=SystemmonitorCurrentLinkSpeed" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IFloatEx& SystemmonitorCurrentLinkSpeed;

        //@}


        //! \name Categories: CoaXPress
        //@{
        /*!
            \brief Camera scan mode - Applies to: CoaXPress

            This parameter informs on the current transfer mode, used by the camera. The transfer can be an areascan (= 0) or linescan (= 1) image.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=SystemmonitorCxpImageLineMode" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& SystemmonitorCxpImageLineMode;

        //@}


        //! \name Categories: Miscellaneous
        //@{
        /*!
            \brief  Applies to: CoaXPress

            Disparity 8b 10b errors

            Visibility: Beginner

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=SystemmonitorDisparity8b10bError" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& SystemmonitorDisparity8b10bError;

        //@}


        //! \name Categories: Miscellaneous
        //@{
        /*!
            \brief  Applies to: CoaXPress

            Shows the external power state of the board.

            Visibility: Beginner

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=SystemmonitorExternalPower" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IEnumParameterT<SystemmonitorExternalPowerEnums>& SystemmonitorExternalPower;

        //@}


        //! \name Categories: Miscellaneous
        //@{
        /*!
            \brief  Applies to: CoaXPress

            Returns the current FPGA die temperature.

            Visibility: Beginner

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=SystemmonitorFpgaTemperature" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IFloatEx& SystemmonitorFpgaTemperature;

        //@}


        //! \name Categories: Miscellaneous
        //@{
        /*!
            \brief  Applies to: CoaXPress

            Returns the current FPGA auxiliary Vcc.

            Visibility: Beginner

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=SystemmonitorFpgaVccAux" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IFloatEx& SystemmonitorFpgaVccAux;

        //@}


        //! \name Categories: Miscellaneous
        //@{
        /*!
            \brief  Applies to: CoaXPress

            Returns the current FPGA BRAM Vcc.

            Visibility: Beginner

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=SystemmonitorFpgaVccBram" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IFloatEx& SystemmonitorFpgaVccBram;

        //@}


        //! \name Categories: Miscellaneous
        //@{
        /*!
            \brief  Applies to: CoaXPress

            Returns the current FPGA internal Vcc.

            Visibility: Beginner

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=SystemmonitorFpgaVccInt" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IFloatEx& SystemmonitorFpgaVccInt;

        //@}


        //! \name Categories: Miscellaneous
        //@{
        /*!
            \brief  Applies to: CoaXPress

            Not in table 8b 10b errors

            Visibility: Beginner

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=SystemmonitorNotInTable8b10bError" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& SystemmonitorNotInTable8b10bError;

        //@}


        //! \name Categories: CoaXPress
        //@{
        /*!
            \brief Number of packet buffer overflows - Applies to: CoaXPress

            Represents the number of overflows, if an overflow occurred due to not correctly aligned package order.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=SystemmonitorPacketbufferOverflowCount" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& SystemmonitorPacketbufferOverflowCount;

        //@}


        //! \name Categories: CoaXPress
        //@{
        /*!
            \brief Source of packet buffer overflows - Applies to: CoaXPress

            Represents the port, which has overflows due to not correctly aligned package order.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=SystemmonitorPacketbufferOverflowSource" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& SystemmonitorPacketbufferOverflowSource;

        //@}


        //! \name Categories: Miscellaneous
        //@{
        /*!
            \brief  Applies to: CoaXPress

            Returns the PCIe trained payload size.

            Visibility: Beginner

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=SystemmonitorPcieTrainedPayloadSize" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& SystemmonitorPcieTrainedPayloadSize;

        //@}


        //! \name Categories: Miscellaneous
        //@{
        /*!
            \brief  Applies to: CoaXPress

            Returns the PCIe trained request size.

            Visibility: Beginner

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=SystemmonitorPcieTrainedRequestSize" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& SystemmonitorPcieTrainedRequestSize;

        //@}


        //! \name Categories: Miscellaneous
        //@{
        /*!
            \brief  Applies to: CoaXPress

            Returns the port bit rate

            Visibility: Beginner

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=SystemmonitorPortBitRate" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IFloatEx& SystemmonitorPortBitRate;

        //@}


        //! \name Categories: Miscellaneous
        //@{
        /*!
            \brief  Applies to: CoaXPress

            Shows the current power over CXP state.

            Visibility: Beginner

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=SystemmonitorPowerOverCxpState" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IEnumParameterT<SystemmonitorPowerOverCxpStateEnums>& SystemmonitorPowerOverCxpState;

        //@}


        //! \name Categories: CoaXPress
        //@{
        /*!
            \brief Number of used connections - Applies to: CoaXPress

            The currently used number of CXP ports used in this process.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=SystemmonitorUsedCxpConnections" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& SystemmonitorUsedCxpConnections;

        //@}


        //! \name Categories: CoaXPress
        //@{
        /*!
            \brief Number of trigger acknowledgement packets sent by the camera have been received by the frame grabber - Applies to: CoaXPress

            Number of trigger acknowledgement packets sent by the camera (in response to trigger edge packets sent before) have been received by the frame grabber.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerAcknowledgementCount" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& TriggerAcknowledgementCount;

        //@}


        //! \name Categories: CameraOutSignalMapping
        //@{
        /*!
            \brief Sets the output source to be connected to a camera signal channel - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerCameraOutSelect" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IEnumParameterT<TriggerCameraOutSelectEnums>& TriggerCameraOutSelect;

        //@}


        //! \name Categories: CoaXPress
        //@{
        /*!
            \brief Number of trigger events sent to the camera - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerEventCount" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& TriggerEventCount;

        //@}


        //! \name Categories: OutStatistics
        //@{
        /*!
            \brief Returns whether the  input signal frequency exceeded the maximum specified - Applies to: CoaXPress

            Returns whether the  input signal frequency exceeded the maximum specified. If the parameter returns Yes, signals were dropped.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerExceededPeriodLimits" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IEnumParameterT<TriggerExceededPeriodLimitsEnums>& TriggerExceededPeriodLimits;

        //@}


        //! \name Categories: OutStatistics
        //@{
        /*!
            \brief Clears the Trigger Exceeded Period Limits parameter - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerExceededPeriodLimitsClear" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::ICommandEx& TriggerExceededPeriodLimitsClear;

        //@}


        //! \name Categories: External
        //@{
        /*!
            \brief Input debounce time for trigger input signals (in microseconds) - Applies to: CoaXPress

            Input debounce time for trigger input signals (in microseconds). This parameter allows you to filter out invalid signals by specifying a minimum signal length.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerInDebounce" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IFloatEx& TriggerInDebounce;

        //@}


        //! \name Categories: External
        //@{
        /*!
            \brief Factor by which the trigger input signal is downscaled - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerInDownscale" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& TriggerInDownscale;

        //@}


        //! \name Categories: External
        //@{
        /*!
            \brief Phase for selecting the desired pulse in the sequence - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerInDownscalePhase" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& TriggerInDownscalePhase;

        //@}


        //! \name Categories: External
        //@{
        /*!
            \brief Sets the polarity of the trigger input signal - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerInPolarity" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IEnumParameterT<TriggerInPolarityEnums>& TriggerInPolarity;

        //@}


        //! \name Categories: External
        //@{
        /*!
            \brief Sets the trigger input source - Applies to: CoaXPress

            Sets the trigger input source for external trigger mode.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerInSource" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IEnumParameterT<TriggerInSourceEnums>& TriggerInSource;

        //@}


        //! \name Categories: InStatistics
        //@{
        /*!
            \brief Current frequency of the input pulses - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerInStatisticsFrequency" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IFloatEx& TriggerInStatisticsFrequency;

        //@}


        //! \name Categories: InStatistics
        //@{
        /*!
            \brief Maximum input frequency detected - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerInStatisticsMaximumFrequency" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IFloatEx& TriggerInStatisticsMaximumFrequency;

        //@}


        //! \name Categories: InStatistics
        //@{
        /*!
            \brief Clears the minimum and maximum frequency values - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerInStatisticsMinMaxFrequencyClear" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::ICommandEx& TriggerInStatisticsMinMaxFrequencyClear;

        //@}


        //! \name Categories: InStatistics
        //@{
        /*!
            \brief Minimum input frequency detected - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerInStatisticsMinimumFrequency" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IFloatEx& TriggerInStatisticsMinimumFrequency;

        //@}


        //! \name Categories: InStatistics
        //@{
        /*!
            \brief Sets the polarity of the trigger input signal for statistics - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerInStatisticsPolarity" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IEnumParameterT<TriggerInStatisticsPolarityEnums>& TriggerInStatisticsPolarity;

        //@}


        //! \name Categories: InStatistics
        //@{
        /*!
            \brief Number of input pulses - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerInStatisticsPulseCount" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& TriggerInStatisticsPulseCount;

        //@}


        //! \name Categories: InStatistics
        //@{
        /*!
            \brief Clears the input signal pulse counter - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerInStatisticsPulseCountClear" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::ICommandEx& TriggerInStatisticsPulseCountClear;

        //@}


        //! \name Categories: InStatistics
        //@{
        /*!
            \brief Sets the input source for statistics - Applies to: CoaXPress

            Sets the trigger input source for statistics.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerInStatisticsSource" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IEnumParameterT<TriggerInStatisticsSourceEnums>& TriggerInStatisticsSource;

        //@}


        //! \name Categories: Sequencer
        //@{
        /*!
            \brief Upscales, i e , duplicates, the number of trigger pulses generated externally or via software with the period specified by the Trigger Output Frequency parameter - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerMultiplyPulses" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& TriggerMultiplyPulses;

        //@}


        //! \name Categories: DigitalOutput
        //@{
        /*!
            \brief Sets the output source of Front GPO 0 - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerOutSelectFrontGPO0" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IEnumParameterT<TriggerOutSelectFrontGPO0Enums>& TriggerOutSelectFrontGPO0;

        //@}


        //! \name Categories: DigitalOutput
        //@{
        /*!
            \brief Sets the output source of Front GPO 1 - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerOutSelectFrontGPO1" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IEnumParameterT<TriggerOutSelectFrontGPO1Enums>& TriggerOutSelectFrontGPO1;

        //@}


        //! \name Categories: DigitalOutput
        //@{
        /*!
            \brief Sets the output source of GPO 0 - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerOutSelectGPO0" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IEnumParameterT<TriggerOutSelectGPO0Enums>& TriggerOutSelectGPO0;

        //@}


        //! \name Categories: DigitalOutput
        //@{
        /*!
            \brief Sets the output source of GPO 1 - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerOutSelectGPO1" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IEnumParameterT<TriggerOutSelectGPO1Enums>& TriggerOutSelectGPO1;

        //@}


        //! \name Categories: DigitalOutput
        //@{
        /*!
            \brief Sets the output source of GPO 2 - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerOutSelectGPO2" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IEnumParameterT<TriggerOutSelectGPO2Enums>& TriggerOutSelectGPO2;

        //@}


        //! \name Categories: DigitalOutput
        //@{
        /*!
            \brief Sets the output source of GPO 3 - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerOutSelectGPO3" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IEnumParameterT<TriggerOutSelectGPO3Enums>& TriggerOutSelectGPO3;

        //@}


        //! \name Categories: DigitalOutput
        //@{
        /*!
            \brief Sets the output source of GPO 4 - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerOutSelectGPO4" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IEnumParameterT<TriggerOutSelectGPO4Enums>& TriggerOutSelectGPO4;

        //@}


        //! \name Categories: DigitalOutput
        //@{
        /*!
            \brief Sets the output source of GPO 5 - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerOutSelectGPO5" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IEnumParameterT<TriggerOutSelectGPO5Enums>& TriggerOutSelectGPO5;

        //@}


        //! \name Categories: DigitalOutput
        //@{
        /*!
            \brief Sets the output source of GPO 6 - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerOutSelectGPO6" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IEnumParameterT<TriggerOutSelectGPO6Enums>& TriggerOutSelectGPO6;

        //@}


        //! \name Categories: DigitalOutput
        //@{
        /*!
            \brief Sets the output source of GPO 7 - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerOutSelectGPO7" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IEnumParameterT<TriggerOutSelectGPO7Enums>& TriggerOutSelectGPO7;

        //@}


        //! \name Categories: OutStatistics
        //@{
        /*!
            \brief Number of output pulses - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerOutStatisticsPulseCount" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& TriggerOutStatisticsPulseCount;

        //@}


        //! \name Categories: OutStatistics
        //@{
        /*!
            \brief Clears the output signal pulse counter - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerOutStatisticsPulseCountClear" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::ICommandEx& TriggerOutStatisticsPulseCountClear;

        //@}


        //! \name Categories: OutStatistics
        //@{
        /*!
            \brief Sets the output source for statistics - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerOutStatisticsSource" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IEnumParameterT<TriggerOutStatisticsSourceEnums>& TriggerOutStatisticsSource;

        //@}


        //! \name Categories: Trigger
        //@{
        /*!
            \brief Maximum trigger output frequency - Applies to: CoaXPress

            Maximum trigger output frequency. If the internal generator is used, it will use the frequency specified there. If an external source is used, this value determines the maximum allowed value. External trigger signals exceeding this value will be discarded. For more information, see the applet documentation.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerOutputFrequency" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IFloatEx& TriggerOutputFrequency;

        //@}


        //! \name Categories: PulseFormGenerator0
        //@{
        /*!
            \brief Delay between the input and the output of the pulse form generator - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerPulseFormGenerator0Delay" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IFloatEx& TriggerPulseFormGenerator0Delay;

        //@}


        //! \name Categories: PulseFormGenerator0
        //@{
        /*!
            \brief Downscale factor at the pulse form generator input - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerPulseFormGenerator0Downscale" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& TriggerPulseFormGenerator0Downscale;

        //@}


        //! \name Categories: PulseFormGenerator0
        //@{
        /*!
            \brief Phase for selecting the desired pulse in the sequence - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerPulseFormGenerator0DownscalePhase" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& TriggerPulseFormGenerator0DownscalePhase;

        //@}


        //! \name Categories: PulseFormGenerator0
        //@{
        /*!
            \brief Width of the output signal - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerPulseFormGenerator0Width" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IFloatEx& TriggerPulseFormGenerator0Width;

        //@}


        //! \name Categories: PulseFormGenerator1
        //@{
        /*!
            \brief Delay between the input and the output of the pulse form generator - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerPulseFormGenerator1Delay" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IFloatEx& TriggerPulseFormGenerator1Delay;

        //@}


        //! \name Categories: PulseFormGenerator1
        //@{
        /*!
            \brief Downscale factor at the pulse form generator input - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerPulseFormGenerator1Downscale" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& TriggerPulseFormGenerator1Downscale;

        //@}


        //! \name Categories: PulseFormGenerator1
        //@{
        /*!
            \brief Phase for selecting the desired pulse in the sequence - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerPulseFormGenerator1DownscalePhase" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& TriggerPulseFormGenerator1DownscalePhase;

        //@}


        //! \name Categories: PulseFormGenerator1
        //@{
        /*!
            \brief Width of the output signal - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerPulseFormGenerator1Width" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IFloatEx& TriggerPulseFormGenerator1Width;

        //@}


        //! \name Categories: PulseFormGenerator2
        //@{
        /*!
            \brief Delay between the input and the output of the pulse form generator - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerPulseFormGenerator2Delay" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IFloatEx& TriggerPulseFormGenerator2Delay;

        //@}


        //! \name Categories: PulseFormGenerator2
        //@{
        /*!
            \brief Downscale factor at the pulse form generator input - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerPulseFormGenerator2Downscale" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& TriggerPulseFormGenerator2Downscale;

        //@}


        //! \name Categories: PulseFormGenerator2
        //@{
        /*!
            \brief Phase for selecting the desired pulse in the sequence - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerPulseFormGenerator2DownscalePhase" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& TriggerPulseFormGenerator2DownscalePhase;

        //@}


        //! \name Categories: PulseFormGenerator2
        //@{
        /*!
            \brief Width of the output signal - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerPulseFormGenerator2Width" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IFloatEx& TriggerPulseFormGenerator2Width;

        //@}


        //! \name Categories: PulseFormGenerator3
        //@{
        /*!
            \brief Delay between the input and the output of the pulse form generator - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerPulseFormGenerator3Delay" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IFloatEx& TriggerPulseFormGenerator3Delay;

        //@}


        //! \name Categories: PulseFormGenerator3
        //@{
        /*!
            \brief Downscale factor at the pulse form generator input - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerPulseFormGenerator3Downscale" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& TriggerPulseFormGenerator3Downscale;

        //@}


        //! \name Categories: PulseFormGenerator3
        //@{
        /*!
            \brief Phase for selecting the desired pulse in the sequence - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerPulseFormGenerator3DownscalePhase" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& TriggerPulseFormGenerator3DownscalePhase;

        //@}


        //! \name Categories: PulseFormGenerator3
        //@{
        /*!
            \brief Width of the output signal - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerPulseFormGenerator3Width" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IFloatEx& TriggerPulseFormGenerator3Width;

        //@}


        //! \name Categories: Queue
        //@{
        /*!
            \brief Fill level of the trigger queue - Applies to: CoaXPress


            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerQueueFillLevel" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& TriggerQueueFillLevel;

        //@}


        //! \name Categories: Queue
        //@{
        /*!
            \brief Enables/disable the trigger queue mode - Applies to: CoaXPress

            Enables/disable the trigger queue mode. The maximum output frequency is specified by the Trigger Output Frequency parameter.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerQueueMode" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IEnumParameterT<TriggerQueueModeEnums>& TriggerQueueMode;

        //@}


        //! \name Categories: Trigger
        //@{
        /*!
            \brief Sets the state of the trigger system - Applies to: CoaXPress

            Sets the state of the trigger system. The state of the trigger system is related to the trigger mode. This parameter enables or disables the trigger processing.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerState" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IEnumParameterT<TriggerStateEnums>& TriggerState;

        //@}


        //! \name Categories: CoaXPress
        //@{
        /*!
            \brief Indicates a trigger frequency violation - Applies to: CoaXPress

            Indicates a distance of two trigger edges violating the minimum edge frequency.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=TriggerWaveViolation" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& TriggerWaveViolation;

        //@}


        //! \name Categories: CoaXPress
        //@{
        /*!
            \brief Number of uncorrected errors - Applies to: CoaXPress

            Number of uncorrected errors. Bit[2] indicates multiple byte errors in CXP stream packets.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=UncorrectedErrorCount" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& UncorrectedErrorCount;

        //@}


        //! \name Categories: ROI
        //@{
        /*!
            \brief Width of the image ROI on the host side - Applies to: CoaXPress

            Width of the image ROI on the host side. This parameter is linked with the Width camera parameter. If the camera parameter changes, the host parameter changes accordingly, but not vice versa. The host parameter should not be set to a value different from the camera parameter.

            Visibility: Expert

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=Width" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& Width;

        //@}


        //! \name Categories: Root
        //@{
        /*!
            \brief Write access timeout in milliseconds - Applies to: GigE


            Visibility: Beginner

            The <a href="https://docs.baslerweb.com/?rhcsh=1&rhmapid=WriteTimeout" target="_blank">Basler Product Documentation</a> may provide more information.
        */
        Pylon::IIntegerEx& WriteTimeout;

        //@}


    private:
    //! \cond HIDE_CLASS_METHODS

        //! not implemented copy constructor
        CUniversalTLParams_Params_v6_3_0(CUniversalTLParams_Params_v6_3_0&);

        //! not implemented assignment operator
        CUniversalTLParams_Params_v6_3_0& operator=(CUniversalTLParams_Params_v6_3_0&);

    //! \endcond
    };


    /*!
    \brief A parameter class containing all parameters as members that are available for pylon device transport layers

    The parameter class is used by the \c Pylon::CBaslerUniversalInstantCamera class.
    The \ref sample_ParametrizeCamera_NativeParameterAccess code sample shows how to access camera parameters via the \c Pylon::CBaslerUniversalInstantCamera class.
    */
    class CUniversalTLParams_Params : public CUniversalTLParams_Params_v6_3_0
    {
    //----------------------------------------------------------------------------------------------------------------
    // Implementation
    //----------------------------------------------------------------------------------------------------------------
    protected:
        // If you want to show the following methods in the help file
        // add the string HIDE_CLASS_METHODS to the ENABLED_SECTIONS tag in the doxygen file
        //! \cond HIDE_CLASS_METHODS
        
        //! Constructor
        CUniversalTLParams_Params( void )
        {
        }

        //! Destructor
        ~CUniversalTLParams_Params( void )
        {
        }

        //! Initializes the references
        void _Initialize( GENAPI_NAMESPACE::INodeMap* pNodeMap )
        {
            CUniversalTLParams_Params_v6_3_0::_Initialize( pNodeMap );
        }
        //! \endcond
    };
} // namespace Basler_UniversalTLParams

#endif // BASLER_PYLON_UNIVERSALTLPARAMS_H
