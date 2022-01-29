package project2;

public class UserInfo {
	private String name;
	private String pw;
	private String date;
	public String getName() {
		return name;
	}
	public void setName(String name) {
		this.name = name;
	}
	public String getPw() {
		return pw;
	}
	public void setPw(String pw) {
		this.pw = pw;
	}
	public String getDate() {
		return date;
	}
	public void setDate(String date) {
		this.date = date;
	}
	
	public UserInfo(String name, String pw, String date) {
		super();
		this.name = name;
		this.pw = pw;
		this.date = date;
	}
	
	
}
