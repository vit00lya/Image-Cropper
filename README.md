

Image Cropper
=============

Qt Widget for cropping images.

The original version was ported to the QT 6.0 version. In this demo application, the ability to select multiple images was implemented, and then save them to a folder after closing the program. It is possible to use the selected fragments for subsequent text recognition by neural networks. Additionally, it allows you to transfer the path to the image and the path to the fragment save folder via the command line.

--ru
Исходная версия была портирована на версию QT 6.0. В данном демо-приложении была реализована возможность выбора нескольких изображений, с последующим сохранением их в папке после закрытия программы. Возможно выделенные фрагменты использовать для последующего распознавания текста нейросетями. Дополнительно позволяет передавать путь к изображению и путь к папке сохранения фрагментов через командную строку.

![Screenshot](https://github.com/vit00lya/Image-Cropper/master/2.png)

Using:
-------------

Use ImageCropper - it's easy. First you need to create instance and configure it:

```cpp
// Create instance
imageCropper = new ImageCropper(this);

// If you need to fix proportions of cropping rect
// ... set proportions if needed
imageCropper->setProportion(QSize(4,3));
// ... and fix it
imageCropper->setProportionFixed(true);

// Set image for cropping
imageCropper->setImage(QPixmap(":/img.jpg"));

// Set background color under cropped image pane
imageCropper->setBackgroundColor(Qt::lightGray);

// Set border color of cropping rect helper
imageCropper->setCroppingRectBorderColor(Qt::magenta);
```

And when you need crop image, just do it:

```cpp
QPixmap croppedImage = imageCropper->cropImage();
```

Build
-------------

Qt 6.0 and greater

License
-------------

[GNU LGPL v3](http://www.gnu.org/copyleft/lesser.html)
