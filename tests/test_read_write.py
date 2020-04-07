import las_2g
import os

filenames_in = ["tests/data/data_2014_255_80517711.427000.las",
                "tests/data/data_2015_256_80517712.427000.las",
                "tests/data/data_2016_257_80517713.427000.las"
                ]


def assert_float_equal(a, b, precision=6):
    assert (round(a, precision) == round(b, precision))


def test_read_file():
    data = las_2g.read_las(filenames_in[0])
    assert (len(data) == 1)
    assert (data[0].header.number_of_points == 1400)
    x = [point.x for point in data[0].entries]
    y = [point.y for point in data[0].entries]
    z = [point.z for point in data[0].entries]

    assert_float_equal(min(x), 5.123456)
    assert_float_equal(min(y), 6.123456)
    assert_float_equal(min(z), 7.123456)
    assert_float_equal(max(x), 6.522456)
    assert_float_equal(max(y), 7.522456)
    assert_float_equal(max(z), 8.522456)


def test_write_file():
    data = las_2g.read_las(filenames_in[0])
    data.append(las_2g.read_las(filenames_in[1])[0])
    data.append(las_2g.read_las(filenames_in[2])[0])

    data[1].entries[500].x = 1.0
    data[1].entries[500].y = 2.0
    data[1].entries[500].z = 3.0
    data[1].entries[500].intensity = 234
    data[1].entries[500].quality = 123
    data[1].entries[500].utc_time = 1585756253000000

    temp_file = "test_output.las"
    las_2g.write_las(temp_file, data)
    data = []
    data = las_2g.read_las(temp_file)
    os.remove(temp_file)

    assert_float_equal(data[1].entries[500].x, 1.0)
    assert_float_equal(data[1].entries[500].y, 2.0)
    assert_float_equal(data[1].entries[500].z, 3.0)
    assert(data[1].entries[500].intensity == 234)
    assert(data[1].entries[500].quality == 123)
    assert(data[1].entries[500].utc_time == 1585756253000000)


if __name__ == "__main__":
    test_read_file()
    test_write_file()
