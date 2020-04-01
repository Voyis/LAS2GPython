import las_2g

def assert_float_equal(a, b, precision = 6):
    assert (round (a, precision) == round (b, precision))

def test_read_file():
    filename = "tests/data/data_2014_255_80517711.427000.las"
    data = las_2g.read_las(filename)
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
    
if __name__ == "__main__":
    test_read_file()