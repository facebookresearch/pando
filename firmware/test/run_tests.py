import pytest
import subprocess
import os
import glob

# Grab all of the testbenches, strip off the leading 'tb/' and trailing '.v'
VERILOG_TEST_BENCHES = [str(x)[3:-2] for x in glob.glob('tb/*_tb.v')]

@pytest.mark.parametrize('testbench', VERILOG_TEST_BENCHES)
def test_run_testbench(testbench):
    ret = subprocess.run(
        ['vvp', testbench],
        stdout=subprocess.PIPE
    )

    # Make sure that the process returned successfully
    assert ret.returncode == 0

    # Parse the stdout to check for errors
    stdout_string = ret.stdout.decode('utf-8')
    if 'FAIL' in stdout_string:
        pytest.fail('Found keyword \'FAIL\' in STDOUT')

