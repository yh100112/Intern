package number2;

public class UserInfo {
	private String name;
	private String pw;
	private String date;

	public void setName(String name) {
		this.name = name;
	}
	public String getName() {
		return this.name;
	}
	public void setPw(String pw) {
		this.pw = pw;
	}
	public String getPw() {
		return this.pw;
	}
	public void setDate(String date) {
		this.date = date;
	}
	public String getDate() {
		return this.date;
	}
	
	public UserInfo(String name, String pw, String date) {
		super();
		this.name = name;
		this.pw = pw;
		this.date = date;
	}
}
