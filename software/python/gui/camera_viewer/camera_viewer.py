import sys
import zmq
from PyQt5 import QtCore, QtGui, QtWidgets
from arvr import pando_pb2


JULIET_ZMQ_EGRESS_EP = "tcp://localhost:%d"


class RxThread(QtCore.QThread):
    change_pixmap_signal = QtCore.pyqtSignal(bytes, int, int)

    def __init__(self, port, parent=None, **kwargs):
        super().__init__(parent=parent, **kwargs)

        self._context = zmq.Context.instance()
        self._sub_socket = self._context.socket(zmq.SUB)
        self._sub_socket.connect(JULIET_ZMQ_EGRESS_EP % port)

        self._sub_socket.set_string(zmq.SUBSCRIBE, "pando.camera_image")

        self._run_flag = True

    def run(self):
        while self._run_flag:
            if self._sub_socket.poll(timeout=250):
                self._rx_frame()

    def stop(self):
        self._run_flag = False
        self.wait()

    def _rx_frame(self):
        (topic, payload) = self._sub_socket.recv_multipart(copy=False)
        topic_str = topic.bytes.partition(b"\0")[0].decode("utf-8")

        # Deserialize packet
        protobuf_rx = pando_pb2.Packet()
        protobuf_rx.ParseFromString(payload.buffer)

        # Sanity check packet
        assert protobuf_rx.HasField("header")
        assert protobuf_rx.HasField("payload")
        assert protobuf_rx.payload.HasField("camera_image")

        img_proto = protobuf_rx.payload.camera_image
        self.change_pixmap_signal.emit(
            img_proto.image_data, img_proto.width, img_proto.height,
        )


class Display(QtWidgets.QLabel):
    def __init__(self, port, parent=None, **kwargs):
        super().__init__(parent=parent, **kwargs)

        self.setWindowTitle("Camera Feed")
        self._thread = RxThread(port, self)
        self._thread.change_pixmap_signal.connect(self.update_image)
        self._thread.start()

    @QtCore.pyqtSlot(bytes, int, int)
    def update_image(self, data, width, height):
        img = QtGui.QImage(data, width, height, QtGui.QImage.Format_Grayscale8,)
        self.resize(width, height)
        pixmap = QtGui.QPixmap.fromImage(img)
        self.setPixmap(QtGui.QPixmap.fromImage(img))

    def closeEvent(self, event):
        self._thread.stop()
        event.accept()


def parseArguments(args):
    parsed_args = {"port": 5060}
    i = 0
    while i < len(args):
        if args[i] == "--port":
            parsed_args["port"] = int(args[i + 1])
            i += 1
        i += 1
    return parsed_args


if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    args = parseArguments(sys.argv)
    d = Display(args["port"])
    d.show()
    sys.exit(app.exec_())
