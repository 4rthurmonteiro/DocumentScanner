# DocumentScanner
### Visão Computacional - UFAL 2016.2
***
## Versão Linha de comando - Terminal
### Versão com interface gráfica via electron [Neste link](https://github.com/alexbatista/ElectronVisaoComputacional)
##### Dependências do projeto

Para execução correta do projeto, será necessário a instalação dos seguintes pacotes:
* [opencv](http://opencv.org/) incluindo os módulos extras [opencv_contrib](https://github.com/opencv/opencv_contrib).
* [Tesseract OCR](https://github.com/tesseract-ocr/tesseract) - Para leitura dos caracteres

##### Instalação e configuração das dependências do projeto

###### Tesseract
Na Wiki do repositório do Tesseract no github, há um guia para sua instalação. Os passos podem ser vistos [neste link do github](https://github.com/tesseract-ocr/tesseract/wiki/Compiling) e também [neste link do github](https://github.com/tesseract-ocr/tesseract/wiki).

[Extra: Tutorial de instalação no ubuntu](https://www.webuildinternet.com/2016/06/28/installing-opencv-with-tesseract-text-module-on-ubuntu/)

`Atenção!! O tesseract possui um repositório tessdata, onde estão disponíveis dados de treinamento do tesseract para diversas linguagens.Esses arquivos serão utilizados para fazer o reconhecimento dos caracteres. Baixe o pacote referente a linguagem desejada e copie para a pasta indicada no tutorial.`
###### Opencv + opencv_contrib
Ao instalar o opencv, lembre-se de também instalar os pacotes extras juntos.


##### Execução do projeto (Via Terminal)

Para execução correta do projeto, gere um makefile executando o seguinte comando na pasta do projeto:
```sh
cmake CMakeLists.txt
```
Se tudo ocorreu bem, a resposta do comando deverá ser semelhante a seguinte:
```sh
-- The C compiler identification is GNU 5.4.0
-- The CXX compiler identification is GNU 5.4.0
-- Check for working C compiler: /usr/bin/cc
-- Check for working C compiler: /usr/bin/cc -- works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Detecting C compile features
-- Detecting C compile features - done
-- Check for working CXX compiler: /usr/bin/c++
-- Check for working CXX compiler: /usr/bin/c++ -- works
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Found OpenCV: /usr/local (found version "3.2.0")
-- OpenCV library status:
--     version: 3.2.0
--     libraries: opencv_calib3d;opencv_core;opencv_features2d;opencv_flann;opencv_highgui;opencv_imgcodecs;opencv_imgproc;opencv_ml;opencv_objdetect;opencv_photo;opencv_shape;opencv_stitching;opencv_superres;opencv_video;opencv_videoio;opencv_videostab;opencv_viz;opencv_aruco;opencv_bgsegm;opencv_bioinspired;opencv_ccalib;opencv_contrib_world;opencv_cvv;opencv_datasets;opencv_dnn;opencv_dpm;opencv_face;opencv_freetype;opencv_fuzzy;opencv_hdf;opencv_line_descriptor;opencv_optflow;opencv_phase_unwrapping;opencv_plot;opencv_reg;opencv_rgbd;opencv_saliency;opencv_sfm;opencv_stereo;opencv_structured_light;opencv_surface_matching;opencv_text;opencv_tracking;opencv_xfeatures2d;opencv_ximgproc;opencv_xobjdetect;opencv_xphoto
--     include path: /usr/local/include;/usr/local/include/opencv
-- Configuring done
-- Generating done
-- Build files have been written to: /home/alexbatista/workspace/DocumentScanner

```
Então,ainda dentro da pasta do projeto, execute o makefile gerado através do comando make:
```sh
make
```
Não é necessário nenhum argumento. A saída deverá ser semelhante a:

```sh
[ 50%] Building CXX object CMakeFiles/threshold_homography_example.dir/threshold_homography_example.cpp.o
[100%] Linking CXX executable threshold_homography_example
[100%] Built target threshold_homography_example
```

Para execução da aplicação basta chamar o executável passando como parâmetro o caminho da imagem a ser processada:

```sh
./documentScanner bd/cupom10.jpg
```
Será exibido então, diversas imagens de acordo com cada transformação realizada e o texto reconhecido pelo OCR estará disponível tanto no terminal, como num arquivo de saída ***out.txt***.
