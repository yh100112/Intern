package kr.ac.kopo;

import java.util.Date;
import java.util.HashMap;

public class ProductInfo {
	private String code;
	private String vendor;
	private Date date;
	
	public String getCode() {return code;}
	public void setCode(String code) {this.code = code;}
	public String getVendor() {return vendor;}
	public void setVendor(String vendor) {this.vendor = vendor;}
	public Date getDate() {return date;}
	public void setDate(Date date) {this.date = date;}
	
	public void find() {
		HashMap<String, String> map = new HashMap<>();
		
		map.put("A100", "�Ｚ����");
		map.put("A200", "LG����");
		map.put("A300", "���̴н�");
		
		vendor = map.get(code);
		if(vendor == null) {
			vendor = "��ϵ��� ���� ��ǰ�Դϴ�!!";
		}
		
		date = new Date();
		
	}
	
	
	
}
