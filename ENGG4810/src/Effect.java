
public enum Effect {
	
	NO_EFFECT {
		public String toString() {
			return "No Effect";
		}
	},
	LOWPASS {
		public String toString() {
			return "Low Pass";
		}
	},
	HIGHPASS {
		public String toString() {
			return "High Pass";
		}
	},
	BANDPASS {
		public String toString() {
			return "Band Pass";
		}
	},
	NOTCH {
		public String toString() {
			return "Notch Pass";
		}
	},
	DELAY {
		public String toString() {
			return "Delay";
		}
	},
	ECHO {
		public String toString() {
			return "Echo";
		}
	},
	DECIMATOR {
		public String toString() {
			return "Decimator";
		}
	},
	BITCRUSHER {
		public String toString() {
			return "Bitcrusher";
		}
	},
	BITWISE {
		public String toString() {
			return "Bitwise Knockout";
		}
	};
}
