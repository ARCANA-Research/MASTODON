import numpy as np
import matplotlib.pyplot as plt
from PIL import Image

def read_text_file_to_array(filename):
    with open(filename, 'r') as file:
        lines = file.readlines()

    # Convert lines of text into a 2D list of integers
    data = [list(map(int, line.split())) for line in lines]
    return np.abs(np.array(np.array(data)[1:]).T[1:])

def normalize_pixel_values(array):
    # Normalize pixel values to range [0, 255] if necessary
    min_val = np.min(array)
    max_val = np.max(array)
    if min_val < 0 or max_val > 255:
        array = 255 * (array - min_val) / (max_val - min_val)
    return array.astype(np.uint8)
    
def convert_negative(array):
    output = []
    for row in array:
        converted_row = []
        for elem in row:
            if (elem > 4294967296):
                elem = ~elem + 1
            converted_row.append(elem)
        output.append(converted_row)
    return np.asarray(output)
    
def display_image(array):
    plt.imshow(array, cmap='gray', vmin=0, vmax=255)
    plt.axis('off')  # Turn off axis numbers and ticks
    plt.show()

# Example usage
filename = './output.txt'  # Replace with your text file name
pixel_array = read_text_file_to_array(filename)
normalized_array = normalize_pixel_values(pixel_array)
display_image(normalized_array)

