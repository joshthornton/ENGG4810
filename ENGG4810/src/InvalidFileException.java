
public class InvalidFileException extends RuntimeException {

	private static final long serialVersionUID = 8974955599455820439L;
	
	/**
	 * {@inheritDoc}
	 */
	public InvalidFileException() {
		super();
	}

	/**
	 * {@inheritDoc}
	 */
	public InvalidFileException(String message) {
		super(message);
	}
	
	/**
	 * {@inheritDoc}
	 */
	public InvalidFileException(Exception ex) {
		super(ex);
	}

}
