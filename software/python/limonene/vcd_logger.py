from vcd import VCDWriter
import sys
import logging

from limonene.limonene_module import LimoneneModule

logging.basicConfig(stream=sys.stdout, level=logging.DEBUG)
log = logging.getLogger("vcd")


def _write_vcd(ofile, samples):
    """Writes all collected samples to a VCD file
    """
    with VCDWriter(ofile, timescale="1 ns", date="today") as writer:
        imu_gyro_x = writer.register_var(
            "pandoboxd.imu", "imu_gyro_x", "integer", size=16
        )
        imu_gyro_y = writer.register_var(
            "pandoboxd.imu", "imu_gyro_y", "integer", size=16
        )
        imu_gyro_z = writer.register_var(
            "pandoboxd.imu", "imu_gyro_z", "integer", size=16
        )
        imu_accel_x = writer.register_var(
            "pandoboxd.imu", "imu_accel_x", "integer", size=16
        )
        imu_accel_y = writer.register_var(
            "pandoboxd.imu", "imu_accel_y", "integer", size=16
        )
        imu_accel_z = writer.register_var(
            "pandoboxd.imu", "imu_accel_z", "integer", size=16
        )
        imu_fsync = writer.register_var("pandoboxd.imu", "imu_fsync", "wire", size=1)
        hrm = writer.register_var("pandoboxd.analog", "hrm", "integer", size=16)
        pulse_ox = writer.register_var(
            "pandoboxd.analog", "pulse_ox", "integer", size=16
        )
        resp_belt = writer.register_var(
            "pandoboxd.analog", "resp_belt", "integer", size=16
        )
        power_meter = writer.register_var(
            "pandoboxd.analog", "power_meter", "integer", size=16
        )
        digital_in = writer.register_var(
            "pandoboxd.digital", "digital_in", "wire", size=8
        )
        digital_in_val = [0, 0, 0, 0, 0, 0, 0, 0]
        for topic, sample in samples:
            if topic == "pandoboxd.imu":
                writer.change(
                    imu_gyro_x, sample.header.timestamp, sample.payload.imu.gyro_x
                )
                writer.change(
                    imu_gyro_y, sample.header.timestamp, sample.payload.imu.gyro_y
                )
                writer.change(
                    imu_gyro_z, sample.header.timestamp, sample.payload.imu.gyro_z
                )
                writer.change(
                    imu_accel_x, sample.header.timestamp, sample.payload.imu.accel_x
                )
                writer.change(
                    imu_accel_y, sample.header.timestamp, sample.payload.imu.accel_y
                )
                writer.change(
                    imu_accel_z, sample.header.timestamp, sample.payload.imu.accel_z
                )
                writer.change(
                    imu_fsync, sample.header.timestamp, sample.payload.imu.fsync
                )
            elif topic == "pandoboxd.digital_in":
                dev_id = sample.payload.digital_in.device_id
                edge = sample.payload.digital_in.edge
                if edge:
                    digital_in_val[7 - dev_id] = 1
                else:
                    digital_in_val[7 - dev_id] = 0

                digital_in_str = "".join([str(x) for x in digital_in_val])
                try:
                    writer.change(digital_in, sample.header.timestamp, digital_in_str)
                except Exception as e:
                    log.exception(e)
                    continue
            elif topic == "pandoboxd.hrm":
                writer.change(hrm, sample.header.timestamp, sample.payload.hrm.sample)
            elif topic == "pandoboxd.pulse_ox":
                writer.change(
                    pulse_ox, sample.header.timestamp, sample.payload.pulse_ox.sample
                )
            elif topic == "pandoboxd.resp_belt":
                writer.change(
                    resp_belt, sample.header.timestamp, sample.payload.resp_belt.sample
                )
            elif topic == "pandoboxd.power_meter":
                writer.change(
                    power_meter,
                    sample.header.timestamp,
                    sample.payload.analog_power_meter.sample,
                )


def entry(filename, events, **kwargs):
    # This is horridly space inefficient, but unfortunatley the VCD
    # logger will barf if we give it data with out of order timestamps
    sorted_events = []
    with open(f"{filename}.vcd", "w") as ofile:
        _write_vcd(ofile, events)
    log.info(f"VCD logfile written to {filename}")


mod = LimoneneModule("vcd", entry)
