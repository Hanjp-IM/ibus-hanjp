;;Following is setting of hangul componant for kana
;;
;;

;;define initial consonant
(define hj-choseong-void '("ㅇ"))
(define hj-choseong-k '("ㅋ" "ㄲ"))
(define hj-choseong-g '("ㄱ"))
(define hj-choseong-s '("ㅅ"))
(define hj-choseong-z '("ㅈ"))
(define hj-choseong-t '("ㅌ" "ㄸ"))
(define hj-choseong-d '("ㄷ"))
(define hj-choseong-ch '("ㅊ" "ㅉ"))
(define hj-choseong-dz '("ᅀ"))
(define hj-choseong-n '("ㄴ"))
(define hj-choseong-h '("ㅎ"))
(define hj-choseong-b '("ㅂ"))
(define hj-choseong-p '("ㅍ"))
(define hj-choseong-m '("ㅁ"))
(define hj-choseong-r '("ㄹ"))
(define hj-choseong-w '("ᅌ"))
(define hj-choseong-alist '(
	hj-choseng-void
	hj-choseong-k
	hj-choseong-g
	hj-choseong-s
	hj-choseong-z
	hj-choseong-t
	hj-choseong-d
	hj-choseong-ch
	hj-choseong-dz
	hj-choseong-n
	hj-choseong-h
	hj-choseong-m
	hj-choseong-r
	hj-choseong-w))

;;define middle consonant
(define hj-jungseong-a '("ㅏ"))
(define hj-jungseong-i '("ㅣ"))
(define hj-jungseong-u '("ㅜ" "ㅡ"))
(define hj-jungseong-e '("ㅔ" "ㅐ"))
(define hj-jungseong-o '("ㅗ"))
(define hj-jungseong-ya '("ㅑ"))
(define hj-jungseong-yu '("ㅠ"))
(define hj-jungseong-yo '("ㅛ"))
(define hj-jungseong-oa '("ㅘ"))
(define hj-jungseong-alist '(
	hj-jungseong-a
	hj-jungseong-i
	hj-jungseong-u
	hj-jungseong-e
	hj-jungseong-o
	hj-jungseong-ya
	hj-jungseong-yu
	hj-jungseong-yo
	hj-jungseong-oa))

;;define final consonant
(define hj-jongseong-g '("ㄱ"))
(define hj-jongseong-n '("ㄴ"))
(define hj-jongseong-m '("ㅁ"))
(define hj-jongseong-b '("ㅂ"))
(define hj-jongseong-s '("ㅅ"))
(define hj-jongseong-ng '("ㅇ"))
(define hj-jongseong-alist '(
	hj-jongseong-g
	hj-jongseong-n
	hj-jongseong-m
	hj-jongseong-b
	hj-jongseong-s
	hj-jongseong-ng))

;;define compound middle consonant
;;need to add

;;define special final consonant
(define hj-batchim-nn '("ᄔ"))
(define hj-batchim-ss '("ㅆ"))
(define hj-batchim '(
	hj-batchim-nn
	hj-batchim-ss))

;;define kana phoneme list
(define hj-kana-phoneme-alist '(
	hj-choseong-alist
	hj-jungseong-alist
	hj-jongseong-alist))

;;define compound middle consonant
(define (hj-combine-compound jungseong1 junseong2)
	(let ((entry (assoc (cons jungseong1 jungseong2) hj-compound-jungseong-alist)))
	(and entry (cdr entry))))
