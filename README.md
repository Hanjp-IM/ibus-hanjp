# hanjp-im

## 시작하기
```
git clone https://github.com/ubuntu-kr/hanjp-im.git hanjp-im
```
- [git 간편 설명서](https://rogerdudler.github.io/git-guide/index.ko.html)

## About Hanjp IM
Hanjp IM은 일본어를 한글 자소로 입력하는 방법을 제공하는 입력 메소드입니다. 이 프로젝트는 일본어 발음의 한글 표현, 가나 문자 표현 간의 유연한 전환 방법을 제공하는 것을 목적으로 하고있습니다.

## 라이선스
위 소프트웨어는 LGPL 3.0 라이선스 아래 배포되고있습니다.

## 바깥 고리
- [Hanjp IM Document @ Ubuntu KR Wiki(Korean)](https://wiki.ubuntu-kr.org/index.php/HanJP_IM)

## Test libhanjp
```
cd libhanjp
cp -r ./keyboard/* ~/.local/share/libhangul/keyboard
make test
./a.out
make clean
```

## About Hanjp IM
The hanjp input method is input method for Japenese kana characters that use hangul for type kana string.
Hanjp suggests easy way to type kana for Korean, and purposing affinity between hangul and kana.
This project is currently in development state.

## License
This software is provided under LGPL 3.0, and the documents for technology reference is available under GNU FDL 1.3 or later version.

## Links
- [Hanjp IM Document @ Ubuntu KR Wiki(Korean)](https://wiki.ubuntu-kr.org/index.php/HanJP_IM)
