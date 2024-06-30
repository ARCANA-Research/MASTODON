from PIL import Image

# Open an image file
with Image.open("ichiro.jpg") as img:
    # Resize image to 64x64 pixels
    img_resized = img.resize((64, 64), Image.ANTIALIAS)
    img_gray = img_resized.convert("L")

    pixels = list(img_gray.getdata())

    # Reshape the list of pixels into a 2D array
    pixel_matrix = [pixels[i * 64:(i + 1) * 64] for i in range(64)]

    # Save the pixel values to a text file
    with open("ichiro64x64.txt", "w") as f:
        for row in pixel_matrix:
            f.write(" ".join(map(str, row)) + "\n")
