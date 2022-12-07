# Image-Enhancement-Corner-Preserve-Averaging

Background: While thresholding is a good way to filter an image to remove background noise, it can be faulty and inadvertently remove the corners of the objects.
Another way to remove noise from an image which does not compromise the corners is Averaging. 
Averaging takes the average of each pixels' neighboring pixels  and replaces the pixel with that average value.

My program uses 8 different configurations of neighboring pixels and then chooses the average that is most similar to the pixel that is being looked at. 

Inputs: inFile- greyscale image with header
threshold value
