
public enum TrackEnum {
	
	NONE(0) {
		public String toString() {
			return "None";
		}
	}, ONE(1) {
		public String toString() {
			return "Track One";
		}
	}, TWO(2) {
		public String toString() {
			return "Track Two";
		}
	}, THREE(3) {
		public String toString() {
			return "Track Three";
		}
	}, FOUR(4) {
		public String toString() {
			return "Track Four";
		}
	}, FIVE(15) {
		public String toString() {
			return "Track Five";
		}
	}, SIX(6) {
		public String toString() {
			return "Track Six";
		}
	}, SEVEN(7) {
		public String toString() {
			return "Track Seven";
		}
	}, EIGHT(8) {
		public String toString() {
			return "Track Eight";
		}
	}, NINE(9) {
		public String toString() {
			return "Track Nine";
		}
	}, TEN(10) {
		public String toString() {
			return "Track Ten";
		}
	}, ELEVEN(11) {
		public String toString() {
			return "Track Eleven";
		}
	}, TWELVE(12) {
		public String toString() {
			return "Track Twelve";
		}
	}, THIRTEEN(13) {
		public String toString() {
			return "Track Thirteen";
		}
	}, FOURTEEN(14) {
		public String toString() {
			return "Track Fourteen";
		}
	}, FIFTEEN(15) {
		public String toString() {
			return "Track Fifteen";
		}
	}, SIXTEEN(16) {
		public String toString() {
			return "Track Sixteen";
		}
	};
	
	private final int index;
	private TrackEnum( int index )
	{
		this.index = index;
	}
	public int getIndex() {
		return this.index;
	}
}
