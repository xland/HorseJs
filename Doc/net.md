- 获取Ip地址

```js
let data = await horse.net.getAddress();
console.log(data);
//{data:['fe80::4068:7357:62bc:a3df','10.26.54.55'],ok:true}
```

- 监听网络连接、断开事件
```js
await horse.net.on("connChanged", (data) => {
  console.log(data);
});
//{data: 'disconn', ok: true}
//{data: 'conn', ok: true, type: 'ipv4'}
```