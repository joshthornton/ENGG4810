
public enum Action {
	
	NO_ACTION(0) {
		public String toString() {
			return "No Action";
		}
	},
	EFFECT_ONE_TOGGLE(1) {
		public String toString() {
			return "Toggle Effect One";
		}
	}, EFFECT_TWO_TOGGLE(2) {
		public String toString() {
			return "Toggle Effect Two";
		}
	};
	
	private final int index;
	private Action( int index )
	{
		this.index = index;
	}
	public int getIndex() {
		return this.index;
	}
}
