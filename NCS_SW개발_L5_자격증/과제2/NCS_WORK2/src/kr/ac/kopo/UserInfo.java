package kr.ac.kopo;

public class UserInfo {
	private String name; // 작성자
	private String code; // 비번
	private String date; // 작성일
	public String getName() {
		return name;
	}
	public void setName(String name) {
		this.name = name;
	}
	public String getCode() {
		return code;
	}
	public void setCode(String code) {
		this.code = code;
	}
	public String getDate() {
		return date;
	}
	public void setDate(String date) {
		this.date = date;
	}
	
	public UserInfo(String name, String code, String date) {
		super();
		this.name = name;
		this.code = code;
		this.date = date;
	}
	
	
	
	
}
