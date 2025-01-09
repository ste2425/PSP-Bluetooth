export class BTError extends Error {}
export class BTConnectionGATTConnectionError extends BTError {}
export class BTConnectionAbortedError extends BTError {}
export class BTConnectionUnknownError extends BTError {}
export class BTConnectionVersionMissmatchError extends BTError {
	constructor(public recieved: string, public expected: string) {
		super();
	}
}
export class BTConnectionPrimaryServiceError extends BTError {}
