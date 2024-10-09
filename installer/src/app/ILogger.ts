export interface ILogger {
    clean: () => void,
    writeLine: (value: string) => void,
    write: (value: string) => void,
    prettyPrint: (value: Record<string, any>) => void
  }