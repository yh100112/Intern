<%@ page language="java" contentType="text/html; charset=UTF-8"
    pageEncoding="UTF-8"%>
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title></title>
<style>
	.title { text-align:center; font-weight:bold; }
</style>
</head>
<body>
	<h1>회원 등록</h1>
	<form>
		<table border="1">
			<tr>
				<td class="title">아이디</td>
				<td><input type="text" name="p_id"></td>
			</tr>
			<tr>
				<td class="title">비밀번호</td>
				<td><input type="text" name="p_pw"></td>
			</tr>
			<tr>
				<td class="title">성명</td>
				<td><input type="text" name="c_name"></td>
			</tr>
			<tr>
				<td class="title">이메일</td>
				<td><input type="text" name="c_email"></td>
			</tr>
			<tr>
				<td class="title">연락처</td>
				<td><input type="text" name="c_tel"></td>
			</tr>
			<tr>
				<td colspan="2">
					<button type="submit" formaction="add.jsp">등록</button>
					<a href="custom01_list.jsp"><button type="button">조회</button></a>
				</td>
			</tr>
		</table>
	</form>
</body>
</html>





















