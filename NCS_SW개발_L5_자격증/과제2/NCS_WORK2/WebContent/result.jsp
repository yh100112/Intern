<%@ page language="java" contentType="text/html; charset=EUC-KR" pageEncoding="EUC-KR" import="kr.ac.kopo.*"%>
    
<%
	UserInfo user = new UserInfo("�ѱ���","1004","2019-10-31");
	ProductInfo product = new ProductInfo();
	
	String code = request.getParameter("code");
	
	product.setCode(code);
	product.find();
%>
<!DOCTYPE html>
<html>
<head>
<meta charset="EUC-KR">
<title></title>
</head>
<body>
	<h1>��ǰ�ڵ� ��ȸ ���</h1>
	<hr>
	<table border="1">
		<tr>
				<td colspan="2">������ ��ȸ ���</td>
		</tr>
		<tr>
				<td>��ǰ�ڵ�</td>
				<td><%= product.getCode() %></td>
		</tr>
		<tr>
				<td>�������</td>
				<td><%= product.getVendor() %></td>
		</tr>
		<tr>
				<td>��ȸ�Ͻ�</td>
				<td><%= product.getDate() %></td>
		</tr>
	</table>
	<hr>
	<div>
		�ۼ���: <%= user.getName() %>���:<%= user.getCode() %> �ۼ���:<%= user.getDate() %>
	</div>
</body>
</html>