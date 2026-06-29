# malloc_challenge

## ファイルについて
```malloc_challenge/malloc/malloc.c```を変更・改善

## 現状
既存のコードを見つつ、実装できそうな課題は完成しました！
しかし、ポインタの使い方をあまり理解できていないようで、残り2つの課題で躓いています。

課題内容
```
授業中に触れたmallocの性能改善案を実装してみる
    1 Best-fit mallocに改造しよう
    2 Freelist binを実装しよう
    3 空になったページを返却しよう（munmap_to_system()を適切に呼んでOSにページを返却しよう）
    4 空き領域の右結合を実装しよう
```
1,2は実装完了
3をとばして、4を実装してみていました。

## 1 Best-fit mallocに改造しよう
探しているfree_listのサイズをmetadataが満たしている場合、best変数に保存していく。
bestは”探しているfree_listのサイズをmetadataが満たしている”かつ”既に保存されているbestよりも小さいfree_list”が見つかった場合更新していく
```
  while (metadata) {
    if (metadata->size >= size){
      if (!best || metadata->size < best->size){
        best = metadata;
        best_prev = prev;
      }
    }
    prev = metadata;
    metadata = metadata->next;
  }

  metadata = best; 
  prev = best_prev;
```

## 2 Freelist binを実装しよう
free_listが単方向リストとして繋がっていたため、metadataのsizeを1024毎に区切り、4つの単方向リストを作りました。
```my_malloc()```では、要求サイズに応じて探索を始めるリストを分けています。

## 3 空になったページを返却しよう（munmap_to_system()を適切に呼んでOSにページを返却しよう）
何から手を付けていいかわからず、、、、、

## 4 空き領域の右結合を実装しよう
やりたいことは理解しているのですが、C言語の書き方で苦戦しています
```
// 右側もfree_listだった場合、結合する
void my_merge_right(my_metadata_t *metadata) {
  //metadataのポインタの位置からsize分だけ右に移動したポインタが示すところを見たい
  if (metadata + metadata->size + 1 == metadata->next){
    // free_listがアドレス順じゃないので、これだと右隣を調べられていない
    // アドレス的に見た、右隣を指定しないといけない
    metadata->size = metadata->size + metadata->next->size;
    metadata->next = metadata->next->next;
  }
}
```


