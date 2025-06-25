interface Win {
  startDrag: () => {};
}
interface Horse {
  win: Win;
}
declare var horse: Horse;
