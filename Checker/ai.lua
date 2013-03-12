math.randomseed(os.time())
local random = math.random

local g_monteCarlo = {
	map = {}, 
}

function monteCarloSelect(node)			
	local num_moves = aiclib.getNumberOfMoves(node.value)
	local new_game_state = aiclib.simulate(node.value, random(num_moves)-1)
	local selected_node = noil
	if(g_monteCarlo.map[new_game_state] ~= nil) then
		--print("already exist")
		selected_node = g_monteCarlo.map[new_game_state]		
	else
		--print("not exist yet")
		selected_node = {value = new_game_state, parent = node, result = 0, visit = 0}
	end	
	--print("select : ", selected_node.value)
	return selected_node
end

function monteCarloExpand(node)
	if(g_monteCarlo.map[node.value] == nil) then
		g_monteCarlo.map[node.value] = node
	end
	--print("expand : ", node.value)
	return node
end

function monteCarloSimulate(node)
	local current_node = node
	local num_moves = nil
	local new_game_state = nil
	local result = aiclib.whoWin(current_node.value)
	local count = 0

	while(result == 2) do
		num_moves = aiclib.getNumberOfMoves(current_node.value)
		new_game_state = aiclib.simulate(current_node.value, random(num_moves)-1)
		current_node = {value = new_game_state, parent = current_node, result = 0, visit = 0}
		result = aiclib.whoWin(current_node.value)
		count = count + 1
	end
	print("result of simulation : ", result, count)
	return result
end

function monteCarloBackPropagation(node, result)
	node.result = node.result + result
	node.visit = node.visit + 1
end

function monteCarloSelectFinal(node)
	local num_moves = aiclib.getNumberOfMoves(node.value)	
	local best_move_node = nil	
	local new_game_state = nil
	local turn = aiclib.getTurn(node.value)
	local best_move_avg = nil
	local current_avg = nil

	for i=0, num_moves-1 do
		new_game_state = aiclib.simulate(node.value, i)
		if(g_monteCarlo.map[new_game_state]~= nil ) then
			current_avg = g_monteCarlo.map[new_game_state].result / g_monteCarlo.map[new_game_state].visit
			if((best_move_avg == nil) or (turn==1 and current_avg > best_move_avg) or (turn == 2 and current_avg < best_move_avg)) then
				best_move_avg = current_avg
				best_move_node = g_monteCarlo.map[new_game_state]			
			end
		end
	end
	return best_move_node
end

--menerima state game_state dengan jumlah kemungkinan move sebanyak num_moves dengan waktu proses maksimum sebanyak time
function monteCarlo(game_state, num_moves, time) 			
	--[[
	local start_time = os.time()
	local root_node
	if(g_monteCarlo.map[game_state] ~= nil) then
		root_node = g_monteCarlo.map[game_state]
	else
		root_node = {value = game_state, parent = nil, result = 0, visit = 0}
		g_monteCarlo.map[root_node.value] = root_node
	end			

	local current_node = nil
	local last_node = nil

	while(os.time() - start_time < time) do
		current_node = root_node
		
		while(g_monteCarlo.map[current_node.value] ~= nil) do
			last_node = current_node
			current_node = monteCarloSelect(current_node)
		end
		last_node = monteCarloExpand(last_node)

		local result = monteCarloSimulate(last_node)

		current_node = last_node
		while(current_node ~= nil and g_monteCarlo.map[current_node.value] ~= nil) do
			monteCarloBackPropagation(current_node, result)
			current_node = current_node.parent
		end
	end	
	local best_move = monteCarloSelectFinal(root_node)
	--]]

	math.random(num_moves)
	local ran = math.random(num_moves) - 1 -- zero based index			
	return ran
end