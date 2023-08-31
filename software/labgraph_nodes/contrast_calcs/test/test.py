import contrast_calcs

import unittest
import time
import numpy as np


class TestModuleTest(unittest.TestCase):
    def test_import(self):
        return True

    def test_normalize(self):
        avg = 20 * np.ones((3, 4), dtype="uint8")
        new_fr = 4 * np.ones((3, 4), dtype="uint8")
        norm = np.zeros((3, 4)).astype(np.float32)

        contrast_calcs.normalize(avg, new_fr, norm)

        self.assertAlmostEqual(norm.shape, (3, 4))
        self.assertAlmostEqual(norm[0, 1], 0.2)
        self.assertIsInstance(norm[0, 0], np.float32)

        new_fr = 10 * np.ones((3, 4), dtype="uint8")

        contrast_calcs.normalize(avg, new_fr, norm)

        self.assertAlmostEqual(norm[1, 1], 0.5)

        avg = np.random.randint(low=0, high=20, size=(2304, 2304), dtype=np.uint8)
        new_fr = np.random.randint(low=0, high=20, size=(2304, 2304), dtype=np.uint8)
        norm = np.zeros((2304, 2304)).astype(np.float32)

        contrast_calcs.normalize(avg, new_fr, norm)

        test_norm = np.divide(new_fr, avg)
        np.nan_to_num(test_norm, copy=False)

        for i in range(2304):
            for j in range(2304):
                if test_norm[i, j] > 1000:
                    test_norm[i, j] = 0

        self.assertTrue(np.allclose(norm, test_norm))

    def test_dark_image(self):
        dark_image = 4 * np.ones((3, 4), dtype="uint8")
        frame = 20 * np.ones((3, 4), dtype="uint8")

        contrast_calcs.dark_image(dark_image, frame)

        self.assertAlmostEqual(frame.shape, (3, 4))
        self.assertAlmostEqual(frame[0, 1], 16)
        self.assertIsInstance(frame[0, 0], np.uint8)

        dark_image = 5 * np.ones((3, 4), dtype="uint8")
        frame = 4 * np.ones((3, 4), dtype="uint8")

        contrast_calcs.dark_image(dark_image, frame)

        self.assertAlmostEqual(frame[0, 0], 0)

    def test_rolling_average(self):
        frame = np.ones((2, 2), dtype="uint8")

        cc = contrast_calcs.RollingAverage(frame.size)

        # empty, part full, full, and past full frame history all produce correct result
        self.assertTrue(np.array_equal(cc.Update(frame, 2), [[1, 1], [1, 1]]))
        self.assertTrue(np.array_equal(cc.Update(frame, 2), [[1, 1], [1, 1]]))
        self.assertTrue(np.array_equal(cc.Update(frame, 2), [[1, 1], [1, 1]]))
        self.assertTrue(np.array_equal(cc.Update(frame, 2), [[1, 1], [1, 1]]))

        # oldest frame is correctly forgotten
        output4 = cc.Update(3 * frame, 2)
        self.assertTrue(np.array_equal(output4, [[2, 2], [2, 2]]))

        # Shrinking frame history works
        self.assertTrue(np.array_equal(cc.Update(frame, 1), [[1, 1], [1, 1]]))

        # Expanding frame history after shrinking works
        self.assertTrue(np.array_equal(cc.Update(5 * frame, 3), [[3, 3], [3, 3]]))
        self.assertTrue(np.array_equal(cc.Update(9 * frame, 3), [[5, 5], [5, 5]]))

        # Continuing to expand after reaching equilibrium works
        self.assertTrue(np.array_equal(cc.Update(frame, 5), [[4, 4], [4, 4]]))
        self.assertTrue(np.array_equal(cc.Update(4 * frame, 5), [[4, 4], [4, 4]]))
        self.assertTrue(np.array_equal(cc.Update(frame, 5), [[4, 4], [4, 4]]))

        # Shrinking frame history works
        self.assertTrue(np.array_equal(cc.Update(3 * frame, 2), [[2, 2], [2, 2]]))

    def test_contrast_calculator(self):
        frame = np.array([[1.0, 1.0], [1.0, 1.0]]).astype(np.float32)

        cc = contrast_calcs.ContrastCalculator()

        self.assertAlmostEqual(
            cc.GetAllContrasts(frame, frame, frame, frame, True, False, False, False)[
                0
            ],
            0,
        )

        frame = np.array([[0.0, 1.0], [1.0, 2.0]]).astype(np.float32)

        self.assertAlmostEqual(
            cc.GetAllContrasts(frame, frame, frame, frame, True, False, False, False)[
                0
            ],
            0.707106781,
        )

        frame = np.array([[0.0, 4.0], [8.0, 12.0]]).astype(np.float32)

        self.assertAlmostEqual(
            cc.GetAllContrasts(frame, frame, frame, frame, True, True, False, False)[1],
            0.7453559924,
        )

        frame = np.random.random(size=(2304, 2304)).astype(np.float32)
        frame_mean = np.mean(frame)
        frame_std = np.std(frame)
        frame_contrast = frame_std / frame_mean

        self.assertAlmostEqual(
            cc.GetAllContrasts(frame, frame, frame, frame, True, False, False, False)[
                0
            ],
            frame_contrast,
            places=3,
        )

    def test_performance_normalize(self):
        avg = np.random.randint(low=0, high=255, size=(2304, 2304), dtype="uint8")
        new_fr = np.random.randint(low=0, high=255, size=(2304, 2304), dtype="uint8")
        norm = np.random.rand(2304, 2304).astype(np.float32)
        start_time = time.time()
        num_iters = 100
        for i in range(num_iters):
            contrast_calcs.normalize(avg, new_fr, norm)
        time_elapsed = time.time() - start_time
        print(
            "[Normalize] Time elapsed for %s iterations: %s" % (num_iters, time_elapsed)
        )
        self.assertTrue(True)

    def test_performance_dark_image(self):
        dark_image = np.random.randint(low=0, high=50, size=(2304, 2304), dtype="uint8")
        frame = np.random.randint(low=0, high=255, size=(2304, 2304), dtype="uint8")
        start_time = time.time()
        num_iters = 100
        for i in range(num_iters):
            contrast_calcs.dark_image(dark_image, frame)
        time_elapsed = time.time() - start_time
        print(
            "[DarkImage] Time elapsed for %s iterations: %s" % (num_iters, time_elapsed)
        )
        self.assertTrue(True)

    def test_performance_rolling_avg(self):
        frame = np.random.randint(low=0, high=255, size=(2304, 2304), dtype="uint8")
        ra = contrast_calcs.RollingAverage(frame.size)
        fr_len = 10
        start_time = time.time()
        num_iters = 100
        for i in range(num_iters):
            ra.Update(frame, fr_len)
        time_elapsed = time.time() - start_time
        print(
            "[RollingAverage] Time elapsed for %s iterations: %s"
            % (num_iters, time_elapsed)
        )
        self.assertTrue(True)

    def test_performance_contrast_calc_all(self):
        frame0 = np.random.rand(1900, 1200).astype(np.float32)
        frame1 = np.random.rand(1900, 1200).astype(np.float32)
        frame2 = np.random.rand(1900, 1200).astype(np.float32)
        frame3 = np.random.rand(1900, 1200).astype(np.float32)
        cc = contrast_calcs.ContrastCalculator()
        start_time = time.time()
        num_iters = 100
        for i in range(num_iters):
            cc.GetAllContrasts(frame0, frame1, frame2, frame3, True, True, True, True)
        time_elapsed = time.time() - start_time
        print(
            "[ContrastCalcAll] Time elapsed for %s iterations: %s"
            % (num_iters, time_elapsed)
        )
        self.assertTrue(True)


if __name__ == "__main__":
    unittest.main()
