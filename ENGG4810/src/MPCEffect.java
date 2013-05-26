
public enum MPCEffect {
	
	NO_EFFECT(0, 0) {
		public String toString() {
			return "No Effect";
		}
	},
	LOWPASS(1, 1) {
		public String toString() {
			return "Low Pass";
		}
	},
	HIGHPASS(2, 2) {
		public String toString() {
			return "High Pass";
		}
	},
	BANDPASS(4, 3) {
		public String toString() {
			return "Band Pass";
		}
	},
	NOTCH(8, 4) {
		public String toString() {
			return "Notch Pass";
		}
	},
	DELAY(16, 5) {
		public String toString() {
			return "Delay";
		}
	},
	ECHO(32, 6) {
		public String toString() {
			return "Echo";
		}
	},
	DECI_BIT(64, 7) {
		public String toString() {
			return "Decimator / Bitcrusher";
		}
	},
	BITWISE(128, 8) {
		public String toString() {
			return "Bitwise Knockout";
		}
	};
	private final int index;
	private final int arrIndex;
	private MPCEffect( int index, int arrIndex )
	{
		this.index = index;
		this.arrIndex = arrIndex;
	}
	
	public int getArrIndex() {
		return arrIndex;
	}
	
	public int getIndex() {
		return this.index;
	}
	public static MPCEffect indexToEnum( int index )
	{
		switch( index )
		{
		case 1:
			return LOWPASS;
		case 2:
			return HIGHPASS;
		case 4:
			return BANDPASS;
		case 8:
			return NOTCH;
		case 16:
			return DELAY;
		case 32:
			return ECHO;
		case 64:
			return DECI_BIT;
		case 128:
			return BITWISE;
		default:
			return NO_EFFECT;
		}
	}
}
