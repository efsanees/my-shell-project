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

