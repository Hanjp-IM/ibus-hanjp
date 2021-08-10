Hanjp Input Method
=====================
## Getting Started
```
git clone --recurse-submodules https://github.com/Hanjp-IM/hanjp-im.git
cd hanjp-im
# Then, you can build engine mannually
mkdir build
cd build
cmake ..
make
```
Although you can build it mannually, we recommend to use IDE such as VS Code, CLion.

## About Hanjp project
The Hanjp Input Method project is a IME project that aims to give a Hangul jaso Japanese editing enviornment in Korean users friendly form. With a hanjp engine that fits in your environment, you can edit a Kana document with a Hangul jaso. This project supports(but not limited to) Linux platform.

## Dependencies
Currently, the software depends on two packages.
```
libhangul
libibus
```
We can install packages with fllowing command.
```
# On Ubuntu
sudo apt update && sudo apt install libibus-1.0-dev libhangul-dev
```

## License
It is noted in [License](https://github.com/Hanjp-IM/hanjp-im/blob/master/LICENSE)

## Links
- [Hanjp Wiki](https://github.com/ubuntu-kr/hanjp-im/wiki)
- [git hand book](https://rogerdudler.github.io/git-guide/index.ko.html)

> 시연 영상을 YouTube 에서 보려면 아래 이미지를 누르세요    
> Click the image below to watch hanjp-im demo on YouTube   
> [![Working Hanjp IM prototype demo video](https://img.youtube.com/vi/B85Ioo3Hrio/0.jpg)](https://www.youtube.com/watch?v=B85Ioo3Hrio)
