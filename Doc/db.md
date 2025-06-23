数据库

- 打开数据库（不存在则创建）
> 数据库默认创建在应用程序数据目录，比如：<br />
> C:\Users\liuxiaolun\AppData\Roaming\HorseJs\\db.db <br />
> 第二个参数，指定是否把数据库保存在应用程序数据目录 <br />
> 注意本方法的返回值，其他数据库操作要用到此方法返回的数据库路径
```js
let data = await horse.db.open("db.db",true);  //第二个参数可以不用传递
//let data = await horse.db.open("D:\\db.db",false);
//let data = await horse.db.open("db.db",false);
console.log(data);
// {"data":"db.db","ok":true}
// {"data":"D:\\db.db","ok":true}
// {"data":"D:\\project\\HorseJs\\x64\\Debug\\db.db", "ok": true}
```

- 关闭数据库
```js
let data = await horse.db.close("db.db");
console.log(data);
```

- 执行Sql语句
```js
let data = await horse.db.sql("SELECT * FROM users;","db.db");
console.log(data);
```

- 删除数据库
> 如果数据库没有关闭，此操作会默认关闭数据库 
```js
let data = await horse.db.del("db.db",true);   //第二个参数可以不用传递
console.log(data);
```