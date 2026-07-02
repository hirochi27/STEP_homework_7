# malloc_challenge

## ファイルについて
```malloc_challenge/malloc/malloc.c```を変更・改善

## 現状
課題内容
```
授業中に触れたmallocの性能改善案を実装してみる
    1 Best-fit mallocに改造しよう
    2 Freelist binを実装しよう
    3 空になったページを返却しよう（munmap_to_system()を適切に呼んでOSにページを返却しよう）
    4 空き領域の右結合を実装しよう
```
1,2のみ実装時のスコア
timeは長くなったものの、Utilizationが大分上昇した。しかし、challenge1,2はむしろ下がってしまった。
```
====================================================
Challenge #1    |   simple_malloc =>       my_malloc
--------------- + --------------- => ---------------
       Time [ms]|              23 =>            3093
Utilization [%] |              70 =>              65
====================================================
Challenge #2    |   simple_malloc =>       my_malloc
--------------- + --------------- => ---------------
       Time [ms]|              30 =>            1683
Utilization [%] |              40 =>              31
====================================================
Challenge #3    |   simple_malloc =>       my_malloc
--------------- + --------------- => ---------------
       Time [ms]|             321 =>            2026
Utilization [%] |               8 =>              44
====================================================
Challenge #4    |   simple_malloc =>       my_malloc
--------------- + --------------- => ---------------
       Time [ms]|           65002 =>           16148
Utilization [%] |              16 =>              72
====================================================
Challenge #5    |   simple_malloc =>       my_malloc
--------------- + --------------- => ---------------
       Time [ms]|           46362 =>           10136
Utilization [%] |              15 =>              72

Challenge done!
Please copy & paste the following data in the score sheet!
3093,65,1683,31,2026,44,16148,72,10136,72,
```

1～4実装
challenge1,2,3のUtilizationは改善したものの、challenge4,5は下がってしまった。
```
====================================================
Challenge #1    |   simple_malloc =>       my_malloc
--------------- + --------------- => ---------------
       Time [ms]|              38 =>            2978
Utilization [%] |              70 =>              70
====================================================
Challenge #2    |   simple_malloc =>       my_malloc
--------------- + --------------- => ---------------
       Time [ms]|              31 =>            1964
Utilization [%] |              40 =>              39
====================================================
Challenge #3    |   simple_malloc =>       my_malloc
--------------- + --------------- => ---------------
       Time [ms]|             421 =>            2120
Utilization [%] |               8 =>              51
====================================================
Challenge #4    |   simple_malloc =>       my_malloc
--------------- + --------------- => ---------------
       Time [ms]|           74000 =>            7058
Utilization [%] |              16 =>              51
====================================================
Challenge #5    |   simple_malloc =>       my_malloc
--------------- + --------------- => ---------------
       Time [ms]|           58971 =>            4369
Utilization [%] |              15 =>              53

Challenge done!
Please copy & paste the following data in the score sheet!
2978,70,1964,39,2120,51,7058,51,4369,53,
```

### 1 Best-fit mallocに改造しよう
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

### 2 Freelist binを実装しよう
free_listが単方向リストとして繋がっていたため、metadataのsizeを1024毎に区切り、4つの単方向リストを作りました。
```my_malloc()```では、要求サイズに応じて探索を始めるリストを分けています。


### 3 空になったページを返却しよう（munmap_to_system()を適切に呼んでOSにページを返却しよう）
右結合実装後に実装
free_listのmetadataのサイズが4096になった場合```munmap_to_system```を呼び出してページをメモリに返却


### 4 空き領域の右結合を実装しよう
```my_merge_right```として実装
metadataをfree listに追加する前に、メモリ上の右側にfree listが存在するか確認。
存在した場合は、既存のfree listから削除し、metadataのサイズを右のfree listと結合した大きさにする

## ポインタ100マス計算
<img width="2504" height="3576" alt="1000035342" src="https://github.com/user-attachments/assets/d01c74f8-8c67-44f3-8cf9-c6f09c635caa" />


