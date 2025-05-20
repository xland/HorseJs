export namespace Horse {
  export type EventHandler = (...args: any[]) => void;
  export type EventListener = (event: string, data: unknown) => void;
  export interface EventOptions {
    once?: boolean;
    priority?: number;
  }
}
