
public enum MPCEffect {
	
	NO_EFFECT(0) {
		public String toString() {
			return "No Effect";
		}
	},
	LOWPASS(1) {
		public String toString() {
			return "Low Pass";
		}
	},
	HIGHPASS(2) {
		public String toString() {
			return "High Pass";
		}
	},
	BANDPASS(3) {
		public String toString() {
			return "Band Pass";
		}
	},
	NOTCH(4) {
		public String toString() {
			return "Notch Pass";
		}
	},
	DELAY(5) {
		public String toString() {
			return "Delay";
		}
	},
	ECHO(6) {
		public String toString() {
			return "Echo";
		}
	},
	DECI_BIT(7) {
		public String toString() {
			return "Decimator / Bitcrusher";
		}
	},
	BITWISE(8) {
		public String toString() {
			return "Bitwise Knockout";
		}
	};
	private final int index;
	private MPCEffect( int index )
	{
		this.index = index;
	}
	public int getIndex() {
		return this.index;
	}
}
