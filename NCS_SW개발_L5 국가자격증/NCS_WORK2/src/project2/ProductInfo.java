package project2;

import java.util.Date;
import java.util.HashMap;
import java.util.Map;

public class ProductInfo {
	HashMap<String, String> map = new HashMap<>();
	
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
		map.put("A100","SAMSUNG");
		map.put("B100","LG");
		map.put("C100","LOTTE");
		
		vender = map.get(code);
		if(vender == null) {
			vender = "등록되지 않은 제품 입니다!!";
		}
		
		date = new Date();
	}
}





















