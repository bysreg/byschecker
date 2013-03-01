
--menerima state game_state dengan jumlah kemungkinan move sebanyak num_moves dengan waktu proses maksimum sebanyak time
function monteCarlo(game_state, num_moves, time) 
	math.randomseed(os.time())
	math.random(num_moves);
	local ran = math.random(num_moves)	
	return ran
end