import { Eventer } from "./Eventer";

export class Window extends Eventer {
  on(eventName: string, callback: EventHandler): void {
    super.on(`window_${eventName}`, callback);
  }
}
