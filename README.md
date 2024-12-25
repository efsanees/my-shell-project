EFSANE SİNANOĞLU G221210034 1-A
MELİKE GÜN B221210089 1-B
ALPER ZEYBEK B221210025 1-A
MUSTAFA ALPEREN AKÇA B221210014 1-B
SEFA MALKOÇ B221210043 1-A

# Kabuk Programı

Bu proje, Linux üzerinde çalışacak bir kabuk programı geliştirmeyi amaçlamaktadır. Program, aşağıdaki işlevleri yerine getirebilmektedir:

- Komut istemi (`> `) ile kullanıcıdan komut almak.
- `quit` komutunu desteklemek.
- Giriş ve çıkış yönlendirmesi.
- Boru (pipe) ile komutlar arası veri iletimi.
- Arka planda komut çalıştırma.

### Derleme

Projeyi derlemek için terminal üzerinden şu komutu çalıştırabilirsiniz:

make

### Kullanım

Kabuk programı çalıştırıldıktan sonra, komut istemine komutlar girilebilir. Örneğin:

- `ls -l`
- `cat < dosya.txt`
- `echo 'Hello' > output.txt`
- `sleep 5 &`
- `quit`


