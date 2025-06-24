数据库

- 打开数据库（不存在则创建）
> 第一个参数为数据库路径，可以是相对路径，也可以是绝对路径：<br />
> 如果第一个参数为相对路径，且第二个参数的值为 true 则相对路径是应用程序数据目录的相对路径，比如：<br />
> C:\Users\liuxiaolun\AppData\Roaming\HorseJs\\db.db <br />
> 如果第一个参数为相对路径，且第二个参数的值为 false 则相对路径是应用程序 exe 文件所在目录的相对路径 <br />
> 第二个参数默认值为 true <br />
> 如果第一个参数为相对路径，且第二个参数的值为 false ，返回值会返回创建的数据库的绝对路径。 <br />
> 关闭数据库、执行SQL指令、删除数据库等操作均需要提供数据库路径。 <br />
> 当第一个参数为相对路径，且第二个参数的值为 false 时，关闭数据库、执行SQL指令、删除数据库你应该使用数据库的绝对路径
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
> 如果数据库没有关闭，此操作会默认关闭数据库  <br />
> 第二个参数默认值为 true 
```js
let data = await horse.db.del("db.db",true); 
console.log(data);
```