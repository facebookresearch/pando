import sys
from datetime import datetime
from pathlib import Path
import pypando
from PyQt5 import QtWidgets

from gui.config.config_widget import ConfigWidget


class MainWindow(QtWidgets.QMainWindow):
    def __init__(self, **kwargs):
        super().__init__()

        self.setWindowTitle(f"Pando - Version: {pypando.PandoVersionString}")
        self.setCentralWidget(ConfigWidget(self, **kwargs))

        self.show()


def parse_arguments(args):
    parsed = {}
    for i in range(1, len(args), 2):
        if args[i] == "-c":
            parsed["config_filename"] = args[i + 1]
        elif args[i] == "-d":
            parsed["output_directory"] = args[i + 1]
        elif args[i] == "-a":
            parsed["auto_stop"] = int(args[i + 1])
        elif args[i] == "-f":
            parsed["firmware"] = args[i + 1]
        else:
            sys.exit(2)
    return parsed


def main():
    args = parse_arguments(sys.argv)
    # Generate logfile path based on current date & time
    log_dir = "pando_logs"
    log_name = "pando_{0:%y}_{0:%m}_{0:%d}_{0:%H}{0:%M}{0:%S}_{0:%f}.log".format(
        datetime.now()
    )
    log_path = Path(log_dir, log_name)

    # Open log file
    log_path.parent.mkdir(exist_ok=True)
    pypando.Reporter.OpenLogfile(str(log_path))

    # Log the Pando version
    pypando.LogVersion()

    app = QtWidgets.QApplication(sys.argv)
    win = MainWindow(**args)
    app.exec()


if __name__ == "__main__":
    main()
