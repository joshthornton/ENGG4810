
import com.google.gson.Gson;

public class GSON_test {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		Configuration config = new Configuration();
		
		config.effectOne = Effect.LOWPASS;
		config.effectOne = Effect.HIGHPASS;
		config.buttons[0] = new ButtonConfiguration();
		config.buttons[0].action = Action.EFFECT_ONE_TOGGLE;
		
		Gson gson = new Gson();
		System.out.println( gson.toJson( config, Configuration.class ));

		Configuration config2 = gson.fromJson( gson.toJson( config, Configuration.class ), Configuration.class );
		System.out.println( "button0 action: " + config2.buttons[0].action + " effectOne: " + config2.effectOne );
		
	}

}
