import numpy as np
import matplotlib.pyplot as plt
from PIL import Image

def normalize_pixel_values(array):
    # Normalize pixel values to range [0, 255] if necessary
    min_val = np.min(array)
    max_val = np.max(array)
    if min_val < 0 or max_val > 255:
        array = 255 * (array - min_val) / (max_val - min_val)
    return array.astype(np.uint8)

def create_dft_matrix(N):
    PI = np.pi
    real_matrix = np.zeros((N, N), dtype=np.int32)
    imag_matrix = np.zeros((N, N), dtype=np.int32)
    
    for k in range(N):
        for n in range(N):
            angle = -2 * PI * k * n / N
            scale = 512
            
            realval = ~(int(np.cos(angle) * scale)) + 1
            imagval = ~(int(np.sin(angle) * scale)) + 1
            
            real_matrix[k][n] = realval
            imag_matrix[k][n] = imagval
    
    return real_matrix, imag_matrix
    
# Open an image file
pixels = []
N = 64
with Image.open("ichiro.jpg") as img:
    # Resize image to 64x64 pixels
    img_resized = img.resize((N, N), Image.ANTIALIAS)
    img_gray = img_resized.convert("L")

    pixels = np.asarray(list(img_gray.getdata()))
   
# Reshape the pixel array to match the image dimensions
# Example usage of DFT matrix creation:
pixels = pixels.reshape((N, N))
real_matrix, imag_matrix = create_dft_matrix(N)

real_part = real_matrix @ pixels 
imag_part = imag_matrix @ pixels

pixels  =normalize_pixel_values(real_part @ real_matrix.T - imag_part @ imag_matrix.T)
#pixels  =normalize_pixel_values(real_part @ real_matrix.T)
#pixels = real_part
#pixels = np.log(np.abs(pixels) + 1) 
#pixels = real_matrix

# Display the image
plt.imshow(pixels, interpolation='none')
plt.title('Grayscale Image')
plt.axis('off')  # Hide axes
plt.show()



