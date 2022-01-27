package number2;

import java.util.Date;
import java.util.HashMap;

public class ProductInfo {
	private String code;
	private String vender;
	private Date date;
	

	

	public String getCode() {
		return code;
	}




	public void setCode(String code) {
		this.code = code;
	}




	public String getVender() {
		return vender;
	}




	public void setVender(String vender) {
		this.vender = vender;
	}




	public Date getDate() {
		return date;
	}




	public void setDate(Date date) {
		this.date = date;
	}




	public void find() {
		HashMap<String, String>map = new HashMap<>();
		map.put("A100", "삼성전자");

		vender = map.get(code);
		System.out.printf("%s",vender);
		if(vender == null) {
			vender = "등록되지 않은 제품 입니다!!";
		}
		date = new Date();
	}
	
}
